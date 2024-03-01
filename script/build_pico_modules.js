const Mustache = require('mustache');
const fs = require('fs');
const path = require('path');

function CtoJSType(cType) {

    switch (cType) {
        case "uint32_t":
        case "uint64_t":
        case "int32_t":
        case "int64_t":
            return "number";
        case "bool":
            return "boolean";
        case void (0):
            return "";
        default:
            if (cType.includes("_callback_")) {
                return "function";
            }

            throw new Error(`Unknown js type for ${cType}`);
    }

}

class ModuleData {
    constructor(moduleInfo) {
        this.name = moduleInfo.name;
        this.functions = moduleInfo.functions.map((f => new ModuleFunction(f))).filter(f => f.enabled);
    }
}

class ModuleFunction {
    constructor(functionInfo) {
        this.name = functionInfo.name;
        this.returnType = functionInfo.returnType;
        this.linuxRetVal = functionInfo.linuxRetVal;
        this.enabled = functionInfo.enabled === undefined ? true : functionInfo.enabled;
        this.args = functionInfo.args.map((a, i) => new ModuleFunctionArg(a, i, this));
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

    traceSignature() {
        let argList = this.args?.map(a => a.name) ?? [];
        let printfArgs = this.args?.map(a => "%i").join(', ') ?? '';
        let logArgs = ["JERRY_LOG_LEVEL_TRACE", `"${this.name}(${printfArgs});"`, ...argList];
        return `jerry_port_log(${logArgs.join(', ')});`
    }

    signature() {
        let argList = this.args?.map(a => CtoJSType(a.type)).join(', ') ?? '';
        return `${this.name}(${argList})`;
    }
}

class ModuleFunctionArg {
    constructor(arg, index, parent) {
        this.name = arg.name;
        this.type = arg.type;
        this.index = index;
        this.parent = parent;
    }

    jsType() {
        return CtoJSType(this.type);
    }
}

function generate(modInfo, templateFile, outdir) {

    const templatePath = path.join(__dirname, templateFile);
    const template = fs.readFileSync(templatePath).toString('utf8');

    for (let i = 0; i < modInfo.modules.length; i++) {

        const module = new ModuleData(modInfo.modules[i]);
        const outputFile = path.join(outdir, module.name + ".c");

        console.log(`Generating module '${module.name}' to '${outputFile}'`);

        const cFile = Mustache.render(template, module);
        fs.writeFileSync(outputFile, cFile);
    }
}

module.exports = {
    generate
};