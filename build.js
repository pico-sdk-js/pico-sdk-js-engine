require("jsonc-require");

const fs = require("fs-extra");
const os = require("os");
const path = require("path");
const minimist = require("minimist");
const childProcess = require("child_process");

const pico_modules = require("./script/build_pico_modules");

const package = require("./package.json")
const productName = `${package.name}-${package.version}`;

if (process.env.GITHUB_OUTPUT) {
  fs.appendFileSync(process.env.GITHUB_OUTPUT, `PACKAGE_NAME=${package.name}\nPACKAGE_VERSION=${package.version}\nPRODUCT_NAME=${productName}\n`);
  fs.appendFileSync(process.env.GITHUB_STEP_SUMMARY, `# ${package.name} ${package.version}\n`);
}

// Parse options
var unknownArg = false;
var minimistOpts = { 
  string: ['target', 'genOut'],
  boolean: ['clean', 'cmake', 'make', 'gen', 'run', 'build', 'rebuild', 'test', 'publish', 'debug', 'release'],
  default: {
    target: '?'
  },
  unknown: function(x) { 
    console.error(`::error ::ERROR: Unknown arg '${x}'.`);
    unknownArg = true; 
  } 
};
var argv = minimist(process.argv.slice(2), minimistOpts);

if (argv.build) {
  // argv.gen = true;
  argv.cmake = true;
  argv.make = true;
} else if (argv.rebuild) {
  // argv.gen = true;
  argv.clean = true;
  argv.cmake = true;
  argv.make = true;
}

if (unknownArg) {
  return;
}

if (argv.target === '?') {
  switch (process.arch) {
    case "x64":
    case "x32":
      argv.target = 'linux_x86_64';
    break;
    case "arm64":
      argv.target = 'linux_amd64';
    break;
    default:
      console.error("::error ::unsupported target processor for auto-detection");
    break;
  }
}

const buildPath = path.join(__dirname, "build");
const typesPath = argv.genOut ? path.resolve(argv.genOut) : path.join(buildPath, "types");
const testBuildPath = path.join(buildPath, "test");
const jerryBuildPath = path.join(__dirname, "lib/jerryscript/build");

if (argv.clean) {
  clean();
}

if (argv.gen) {
  const linuxModulePath = path.join(__dirname, 'src/os/linux/modules/');
  const rp2xxxModulePath = path.join(__dirname, 'src/os/rp2xxx/modules/');
  const nativeModules = require('./script/native-modules.jsonc');

  pico_modules.generate(nativeModules, 'modules.handlebars', linuxModulePath, 'linux');
  pico_modules.generate(nativeModules, 'modules.handlebars', rp2xxxModulePath, 'rp2xxx');

  fs.ensureDirSync(typesPath);
  pico_modules.generateTypes(nativeModules, 'typescript_types.handlebars', typesPath, 'index.d.ts');
  pico_modules.generateTypes(nativeModules, 'typescript_types_tests.handlebars', typesPath, 'pico-sdk-js-tests.ts');
}

if (argv.cmake) {
  cmake();
}

if (argv.make) {
  make();
}

if (argv.test) {
  test();
}

if (argv.publish) {
  publish();
}

if (argv.debug) {
  startDebugServer();
}

if (argv.run) {
  run();
} 

function clean() {
  fs.removeSync(buildPath);
  fs.removeSync(jerryBuildPath);
}

function cmake() {

  // cmake everything
  const params = [
    "..", 
    `-DTARGET_NAME=${package.name}`,
    `-DTARGET_VERSION=${package.version}`,
    `-DTARGET_OS=${argv.target}`
  ];

  if (argv.release) {
    params.push("-DBUILD_TYPE=MinSizeRel")
  } else {
    params.push("-DBUILD_TYPE=Debug")
  }

  cmd(buildPath, "cmake", params);
}

function make() {

  // make everything
  const cores = os.cpus().length;
  cmd(buildPath, "make", [`-j${cores}`]);
}

function test() {
  if (argv.target === "rp2xxx") {
    console.error('::error ::Running tests on rp2xxx is not supported.');
    process.exit(1);
}

  cmd(testBuildPath, 'ctest'), ['--verbose'];
}

function publish() {

  // sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program pico-sdk-js-0.0.1.elf verify reset exit"
  const exeName = `${productName}.elf`;
  cmd(buildPath, "sudo", ["openocd", "-f", "interface/cmsis-dap.cfg", "-f", "target/rp2040.cfg", "-c", "adapter speed 5000", "-c", `program ${exeName} verify reset exit`]);
}

function startDebugServer() {

  // sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"
  cmd(buildPath, "sudo", ["openocd", "-f", "interface/cmsis-dap.cfg", "-f", "target/rp2040.cfg", "-c", "adapter speed 5000"]);
}

function run() {
  let processName = `./${productName}`;
  cmd(buildPath, processName, []);
}

function cmd(wd, cmd, args) {
  fs.ensureDirSync(wd);
  process.chdir(wd);

  try {
    
    let output = childProcess.spawnSync(cmd, args, { stdio: "inherit" });
    if (output.status !== 0) {
      console.error(`::error ::Child process returned non-zero status: ${output.status}`);
      process.exit(1);
    }
  } finally {
    process.chdir(__dirname);
  }
}
