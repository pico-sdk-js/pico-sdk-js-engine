// Generate c source from js files

const fs = require("fs-extra");
const os = require("os");
const path = require("path");
const minimist = require("minimist");
const childProcess = require("child_process");

const package = require("./package.json")

// Parse options
var argv = minimist(process.argv.slice(2));

const buildPath = path.join(__dirname, "build");
const jerryBuildPath = path.join(__dirname, "lib/jerryscript/build");

const target_os = argv.os || "linux";

if (argv.clean) {
  clean();
} else if (argv.cmake) {
  cmake();
} else if (argv.make) {
  make();
} else if (argv.rebuild) {
  clean();
  build();
} else {
  build();
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
    `-DTARGET_OS=${target_os}`
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

function build() {
  cmake();
  make();
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
