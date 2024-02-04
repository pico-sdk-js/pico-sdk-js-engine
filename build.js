// Generate c source from js files

const fs = require("fs-extra");
const os = require("os");
const path = require("path");
const minimist = require("minimist");
const childProcess = require("child_process");

const package = require("./package.json")

// Parse options
var unknownArg = false;
var minimistOpts = { 
  string: ['target'],
  boolean: ['clean', 'cmake', 'make', 'run', 'build', 'rebuild', 'publish', 'debug'],
  default: {
    target: 'linux'
  },
  unknown: function(x) { 
    console.error(`ERROR: Unknown arg '${x}'.`);
    unknownArg = true; 
  } 
};
var argv = minimist(process.argv.slice(2), minimistOpts);

if (argv.build) {
  argv.cmake = true;
  argv.make = true;
} else if (argv.rebuild) {
  argv.clean = true;
  argv.cmake = true;
  argv.make = true;
}

console.log(argv);

if (unknownArg) {
  return;
}

const buildPath = path.join(__dirname, "build");
const jerryBuildPath = path.join(__dirname, "lib/jerryscript/build");

if (argv.clean) {
  clean();
}

if (argv.cmake) {
  cmake();
}

if (argv.make) {
  make();
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
  cmd(buildPath, "cmake", params);
}

function make() {

  // make everything
  const cores = os.cpus().length;
  cmd(buildPath, "make", [`-j${cores}`]);
}

function publish() {

  // sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program pico-sdk-js-0.0.1.elf verify reset exit"
  const exeName = `${package.name}-${package.version}.elf`;
  cmd(buildPath, "sudo", ["openocd", "-f", "interface/cmsis-dap.cfg", "-f", "target/rp2040.cfg", "-c", "adapter speed 5000", "-c", `program ${exeName} verify reset exit`]);
}

function startDebugServer() {

  // sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"
  cmd(buildPath, "sudo", ["openocd", "-f", "interface/cmsis-dap.cfg", "-f", "target/rp2040.cfg", "-c", "adapter speed 5000"]);
}

function run() {
  let processName = `./${package.name}-${package.version}`;
  cmd(buildPath, processName, []);
}

function cmd(wd, cmd, args) {
  fs.ensureDirSync(wd);
  process.chdir(wd);

  try {
    let output = childProcess.spawnSync(cmd, args, { stdio: "inherit" });
    if (output.status !== 0) {
      console.error(`Child process returned non-zero status: ${output.status}`);
      process.exit(1);
    }
  } finally {
    process.chdir(__dirname);
  }
}
