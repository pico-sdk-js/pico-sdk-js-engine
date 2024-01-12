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

if (argv.clean) {
  clean();
} else {
  build();
}

function clean() {
  fs.removeSync(buildPath);
  fs.removeSync(srcGenPath);
}

function build() {
  // ensure /build
  fs.ensureDirSync(buildPath);

  // execute cmake and make
  process.chdir(buildPath);
  const params = [
    "..", 
    `-DTARGET_NAME=${package.name}`,
    `-DTARGET_VERSION=${package.version}`
  ];

  // build everything
  const cores = os.cpus().length;
  cmd("cmake", params);
  cmd("make", [`-j${cores}`]);
  process.chdir(__dirname);
}

function cmd(cmd, args) {
  childProcess.spawnSync(cmd, args, { stdio: "inherit" });
}
