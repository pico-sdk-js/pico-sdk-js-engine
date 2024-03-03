const fs = require('fs');
const path = require('path');
const Handlebars = require('handlebars');

function CtoJSType(cType) {

    switch (cType) {
        case "uint8_t":
        case "uint16_t":
        case "uint32_t":
        case "uint64_t":
        case "int8_t":
        case "int16_t":
        case "int32_t":
        case "int64_t":
        case "uint":
        case "int":
        case "float":
            return "number";
        case "bool":
            return "boolean";
        case "void":
            return "";
        default:
            if (cType.startsWith('enum ')) {
                return "number";
            } else if (cType.includes("_callback_")) {
                return "function";
            }

            throw new Error(`Unknown js type for ${cType}`);
    }
}

function getDefaultValue(cType) {

    switch (cType) {
        case "uint8_t":
        case "uint16_t":
        case "uint32_t":
        case "uint64_t":
        case "int8_t":
        case "int16_t":
        case "int32_t":
        case "int64_t":
        case "uint":
        case "int":
        case "float":
            return "0";
        case "bool":
            return "true";
        case "void":
            return "";
        default:
            throw new Error(`Unknown default for ${cType}`);
    }
}

class ModuleData {
    constructor(moduleInfo, target) {
        this.target = target;
        this.name = moduleInfo.name;
        this.functions = moduleInfo.functions.map((f => new ModuleFunction(f)));
    }
}

const fnRegEx = /^(?<retType>\w+) (?<name>\w+) \(((?<arg1Type>(enum )?\w+) (?<arg1Name>\w+))?(, (?<arg2Type>(enum )?\w+) (?<arg2Name>\w+))?(, (?<arg3Type>(enum )?\w+) (?<arg3Name>\w+))?(, (?<arg4Type>(enum )?\w+) (?<arg4Name>\w+))?(, (?<arg5Type>(enum )?\w+) (?<arg5Name>\w+))?\)$/;

class ModuleFunction {
    constructor(functionInfo) {
        const match = fnRegEx.exec(functionInfo.fn);
        if (match == null) {
            throw new Error(`Invalid format: ${functionInfo.fn}`);
        }

        this.name = match.groups.name;
        this.returnType = match.groups.retType;

        const args = [];
        for (let i = 1; i <= 5; i++) {
            const typeKey = `arg${i}Type`;
            const nameKey = `arg${i}Name`;
            if (match.groups[typeKey]) {
                args.push({
                    name: match.groups[nameKey],
                    type: match.groups[typeKey]
                });
            } else {
                break;
            }
        }

        this.args = args.map((a) => new ModuleFunctionArg(a));
        this.linuxRetVal = functionInfo.linuxRetVal ?? getDefaultValue(this.returnType);
        this.callback = functionInfo.callback;
    }

    jsReturnType() {
        return CtoJSType(this.returnType);
    }

    functionType() {
        if (this.callback && this.argCount() === 1) {
            return "function_singleton_callback";
        } else if (this.callback && this.argCount() === 2) {
            return "function_callback";
        } else {
            return "function_standard";
        }
    }

    argCount() {
        return this.args.length;
    }

    argNames() {
        return this.args?.map(a => a.name).join(', ') ?? ''
    }

    callbackWrapper() {
        switch (this.callback) {
            case "CALLBACK_HARDWARE_ALARM":
                return "hardware_alarm_set_callback_wrapper";
            default:
                throw new Error(`Unknown callback type: ${this.callback}`);
        }
    }

    traceSignature() {
        if (this.callback && this.argCount() === 1) {
            return `jerry_port_log(JERRY_LOG_LEVEL_TRACE, "${this.name}([Function])");`
        } else if (this.callback && this.argCount() === 2) {
            return `jerry_port_log(JERRY_LOG_LEVEL_TRACE, "${this.name}(%d, [Function])", callback_id);`
        } else {
            let argList = this.args?.map(a => a.name) ?? [];
            let printfArgs = this.args?.map(a => "%i").join(', ') ?? '';
            let logArgs = ["JERRY_LOG_LEVEL_TRACE", `"${this.name}(${printfArgs});"`, ...argList];
            return `jerry_port_log(${logArgs.join(', ')});`
        }
    }

    signature() {
        let argList = this.args?.map(a => CtoJSType(a.type)).join(', ') ?? '';
        return `${this.name}(${argList})`;
    }
}

class ModuleFunctionArg {
    constructor(arg) {
        this.name = arg.name;
        this.type = arg.type;
    }

    converterFunction() {
        if (this.type.startsWith("enum ")) {
            return "psj_jerry_to_int";
        }

        return `psj_jerry_to_${this.type}`;
    }

    jsType() {
        return CtoJSType(this.type);
    }
}

function generate(modInfo, templateFile, outdir, target) {

    const templatePath = path.join(__dirname, templateFile);
    const templateStr = fs.readFileSync(templatePath).toString('utf8');
    const options = {
        strict: true
    };
    const template = Handlebars.compile(templateStr, options);

    for (let i = 0; i < modInfo.modules.length; i++) {

        const module = new ModuleData(modInfo.modules[i], target);
        const outputFile = path.join(outdir, module.name + ".c");

        console.log(`Generating module '${module.name}' to '${outputFile}'`);

        const cFile = template(module, { allowProtoMethodsByDefault: true });
        fs.writeFileSync(outputFile, cFile);
    }
}

Handlebars.registerHelper('ifEquals', function (arg1, arg2, options) {
    return (arg1 == arg2) ? options.fn(this) : options.inverse(this);
});

Handlebars.registerHelper('ifNotEquals', function (arg1, arg2, options) {
    return (arg1 != arg2) ? options.fn(this) : options.inverse(this);
});

Handlebars.registerPartial('function_standard', fs.readFileSync(path.join(__dirname, 'function_standard.handlebars')).toString('utf8'));
Handlebars.registerPartial('function_callback', fs.readFileSync(path.join(__dirname, 'function_callback.handlebars')).toString('utf8'));
Handlebars.registerPartial('function_singleton_callback', fs.readFileSync(path.join(__dirname, 'function_singleton_callback.handlebars')).toString('utf8'));

module.exports = {
    generate
};