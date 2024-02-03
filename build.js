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
  string: ['os'],
  boolean: ['clean', 'cmake', 'make', 'full', 'run', 'build', 'rebuild', 'publish', 'debug'],
  default: {
    os: 'linux'
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

  if (argv.full) {
    fs.removeSync(jerryBuildPath);
  }
}

function cmake() {
  // ensure /build
  fs.ensureDirSync(buildPath);

  process.chdir(buildPath);

  // cmake everything
  const params = [
    "..", 
    `-DTARGET_NAME=${package.name}`,
    `-DTARGET_VERSION=${package.version}`,
    `-DTARGET_OS=${argv.os}`
  ];
  cmd("cmake", params);

  process.chdir(__dirname);
}

function make() {
  // ensure /build
  fs.ensureDirSync(buildPath);

  process.chdir(buildPath);

  // make everything
  const cores = os.cpus().length;
  cmd("make", [`-j${cores}`]);

  process.chdir(__dirname);
}

function publish() {

  const exeName = `${package.name}-${package.version}.elf`;

  // ensure /build
  fs.ensureDirSync(buildPath);

  process.chdir(buildPath);

  // sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program pico-sdk-js-0.0.1.elf verify reset exit"
  cmd("sudo", ["openocd", "-f", "interface/cmsis-dap.cfg", "-f", "target/rp2040.cfg", "-c", "adapter speed 5000", "-c", `program ${exeName} verify reset exit`]);

  process.chdir(__dirname);
}

function startDebugServer() {

  // ensure /build
  fs.ensureDirSync(buildPath);

  process.chdir(buildPath);

  // sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"
  cmd("sudo", ["openocd", "-f", "interface/cmsis-dap.cfg", "-f", "target/rp2040.cfg", "-c", "adapter speed 5000"]);

  process.chdir(__dirname);
}

function run() {
  process.chdir(buildPath);

  let processName = `./${package.name}-${package.version}`;
  cmd(processName, []);

  process.chdir(__dirname);
}

function cmd(cmd, args) {
  childProcess.spawnSync(cmd, args, { stdio: "inherit" });
}
