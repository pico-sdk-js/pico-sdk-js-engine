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
  string: ['buildType', 'os'],
  boolean: ['clean', 'cmake', 'make', 'rebuild', 'build', 'full', 'run'],
  default: {
    clean: false,
    cmake: true,
    make: true,
    buildType: 'Debug',
    os: 'linux'
  },
  unknown: function(x) { 
    console.error(`ERROR: Unknown arg '${x}'.`);
    unknownArg = true; 
  } 
};
var argv = minimist(process.argv.slice(2), minimistOpts);

if (unknownArg) {
  return;
}

if (argv.build) {
  argv.cmake = true;
  argv.make = true;
} else if (argv.rebuild) {
  argv.clean = true;
  argv.cmake = true;
  argv.make = true;
}

console.log(argv);

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
    `-DTARGET_OS=${argv.os}`,
    `-DBUILD_TYPE=${argv.buildType}`
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

function run() {
  process.chdir(buildPath);

  let processName = `./${package.name}-${package.version}`;
  cmd(processName, []);

  process.chdir(__dirname);
}

function cmd(cmd, args) {
  childProcess.spawnSync(cmd, args, { stdio: "inherit" });
}
