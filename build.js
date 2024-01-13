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
const srcGenPath = path.join(__dirname, "src/gen");
const jerryBuildPath = path.join(__dirname, "lib/jerryscript/build");

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

function clean() {
  fs.removeSync(buildPath);
  fs.removeSync(srcGenPath);

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
    `-DTARGET_VERSION=${package.version}`
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

function cmd(cmd, args) {
  childProcess.spawnSync(cmd, args, { stdio: "inherit" });
}
