const fs = require('fs');
const path = require('path');
const Handlebars = require('handlebars');

function CtoJSType(cType) {

    switch (cType) {
        case "uint32_t":
        case "uint64_t":
        case "int32_t":
        case "int64_t":
            return "number";
        case "bool":
            return "boolean";
        case "void":
            return "";
        default:
            if (cType.includes("_callback_")) {
                return "function";
            }

            throw new Error(`Unknown js type for ${cType}`);
    }
}

class ModuleData {
    constructor(moduleInfo, target) {
        this.target = target;
        this.name = moduleInfo.name;
        this.functions = moduleInfo.functions.map((f => new ModuleFunction(f))).filter(f => f.enabled);
    }
}

const fnRegEx = /^(?<retType>\w+) (?<name>\w+) \(((?<arg1Type>\w+) (?<arg1Name>\w+))?(, (?<arg2Type>\w+) (?<arg2Name>\w+))?(, (?<arg3Type>\w+) (?<arg3Name>\w+))?\)$/;

class ModuleFunction {
    constructor(functionInfo) {
        const match = fnRegEx.exec(functionInfo.fn);
        if (match == null) {
            throw new Error(`Invalid format: ${functionInfo.fn}`);
        }

        this.name = match.groups.name;
        this.returnType = match.groups.retType;

        const args = [];
        if (match.groups.arg1Type) {
            args.push({
                name: match.groups.arg1Name,
                type: match.groups.arg1Type
            });
        }
        if (match.groups.arg2Type) {
            args.push({
                name: match.groups.arg2Name,
                type: match.groups.arg2Type
            });
        }
        if (match.groups.arg3Type) {
            args.push({
                name: match.groups.arg3Name,
                type: match.groups.arg3Type
            });
        }

        this.args = args.map((a) => new ModuleFunctionArg(a));
        this.linuxRetVal = functionInfo.linuxRetVal;
        this.enabled = functionInfo.enabled === undefined ? true : functionInfo.enabled;
        this.callback = functionInfo.callback;
    }

    jsReturnType() {
        return CtoJSType(this.returnType);
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
        if (this.callback) {
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

Handlebars.registerHelper('ifEquals', function(arg1, arg2, options) {
    return (arg1 == arg2) ? options.fn(this) : options.inverse(this);
});

Handlebars.registerHelper('ifNotEquals', function(arg1, arg2, options) {
    return (arg1 != arg2) ? options.fn(this) : options.inverse(this);
});

Handlebars.registerPartial('function_standard', fs.readFileSync(path.join(__dirname, 'function_standard.handlebars')).toString('utf8'));
Handlebars.registerPartial('function_callback', fs.readFileSync(path.join(__dirname, 'function_callback.handlebars')).toString('utf8'));

module.exports = {
    generate
};