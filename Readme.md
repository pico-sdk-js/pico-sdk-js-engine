# PICO-SDK-JS

## Building locally

1. Clone the repo

    ```git clone https://github.com/jt000/pico-sdk-js.git```

2. Load submodules

    ```git submodule update --init```

3. Pull NPM Packages

    ```npm install```

4. Run build

    ```node build --rebuild --target rp2xxx```

    **Build Options**

    * **--target <linux | rp2xxx>** - Sets the target for the build to either linux or rp2xxx (rp2040)
    * **--clean** - Removes previous build folders
    * **--build** - Runs cmake &amp; make and produces an executable package
    * **--rebuild** - Cleans the build folder before running a build
    * **--run** - For 'linux' targets will run the executable after a successful build.
