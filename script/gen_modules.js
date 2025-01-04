
const fs = require("fs-extra");
const path = require("node:path");
const xpath = require('xpath');
const { DOMParser } = require('@xmldom/xmldom');

const doxyGenXmlDir = "/home/jt000/_git/psj/engine/build/pico-sdk/docs/doxygen/xml";

function cleanString(msg) {
    if (!msg) return msg;

    // Ensure message ends in punctuation
    if (!msg.endsWith('.')) msg += '.';

    // Ensure first character is capitalized
    msg = msg.charAt(0).toUpperCase() + msg.slice(1)

    return msg;
}

function cleanType(cType) {
    if (cType.startsWith('enum ')) {
        // convert enums to 'int'
        return 'uint32_t';
    }

    switch (cType) {
        case 'uint':
            return 'uint32_t';

        case 'int':
            return 'int32_t';

        default:
            return cType;
    }
}

function getParams(memberdef) {
    const params = [];
    const xmlParams = xpath.select('param', memberdef);
    for (const xmlparam of xmlParams) {
        const param = {
            type: cleanType(xpath.select('string(type)', xmlparam)),
            name: xpath.select('string(declname)', xmlparam)
        };

        if (param.type !== 'void') {
            param.description = xpath.select(`string(detaileddescription/para/parameterlist/parameteritem[parameternamelist/parametername='${param.name}']/parameterdescription/para)`, memberdef).trim();
            param.description = cleanString(param.description);

            params.push(param);
        }
    }

    return params;
}

const fnRegEx = /^(?<retType>\w+\*?) (?<name>\w+) \(((?<arg1Type>(enum )?\w+\*?) (?<arg1Name>\w+))?(, (?<arg2Type>(enum )?\w+\*?) (?<arg2Name>\w+))?(, (?<arg3Type>(enum )?\w+\*?) (?<arg3Name>\w+))?(, (?<arg4Type>(enum )?\w+\*?) (?<arg4Name>\w+))?(, (?<arg5Type>(enum )?\w+\*?) (?<arg5Name>\w+))?\)$/;

function generateTypeInfo(nativeModules) {
    if (!fs.existsSync(doxyGenXmlDir)) {
        throw new Error('doxygen XML directory does not exist. Run "doxygen Doxyfile" in the "build/pico-sdk/docs" folder to generate.');
    }

    const domParser = new DOMParser();
    const xmls = [];
    process.stdout.write(`Loading Doxygen XMLs...`);
    for (const filePath of fs.readdirSync(doxyGenXmlDir)) {
        if (path.extname(filePath) === ".xml") {
            const fullPath = path.join(doxyGenXmlDir, filePath);
            const xml = fs.readFileSync(fullPath).toString('utf8');
            xmls.push(domParser.parseFromString(xml, "text/xml"));
        }
    }
    process.stdout.write(` Done\n`);

    console.log(`Searching for native modules...`);
    for (const module of nativeModules.modules) {
        for (const fn of module.functions) {
            if (fn.fn && !fn.callback && !fn.external) {
                const match = fnRegEx.exec(fn.fn);
                fn.name = match.groups.name;
                delete fn.fn;
            }

            if (fn.name) {
                console.log(`> Searching for '${fn.name}...`);
                for (const xml of xmls) {
                    const nodes = xpath.select(`//doxygen/compounddef/sectiondef/memberdef[name='${fn.name}']`, xml);
                    if (nodes.length > 0) {
                        fn.description = xpath.select('string(briefdescription/para)', nodes[0]).trim();
                        fn.description = cleanString(fn.description);

                        fn.returnType = xpath.select('string(type)', nodes[0]);
                        if (fn.returnType.startsWith('enum ')) {
                            // convert enums to 'int'
                            fn.returnType = 'int';
                        }

                        fn.returnDescription = xpath.select('string(detaileddescription/para/simplesect[@kind=\'return\']/para)', nodes[0]);
                        fn.returnDescription = cleanString(fn.returnDescription);

                        fn.args = getParams(nodes[0]);

                        console.log(fn);

                        break;
                    }
                }
                console.log('> ... Done');
            }
        }
    }
    console.log(`... Done`);
}

module.exports = {
    generateTypeInfo
};