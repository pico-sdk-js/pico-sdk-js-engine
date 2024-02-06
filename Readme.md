# PICO-SDK-JS

## Setting up your machine

*Note: I'm running my environment on Raspberry Pi OS on a Raspberry Pi 5 ARM64.*

1. Install dotnet 7 (required for GCM)

    *Note: I was unable to install via `apt-get`, so I installed using  dotnet-install.sh*

    See [.NET Scripted Install](https://learn.microsoft.com/en-us/dotnet/core/install/linux-scripted-manual#scripted-install) and [Set Environment Variables](https://learn.microsoft.com/en-us/dotnet/core/install/linux-scripted-manual#set-environment-variables-system-wide) for more info.

    ```bash
    wget https://dot.net/v1/dotnet-install.sh -O dotnet-install.sh
    chmod +x ./dotnet-install.sh
    ./dotnet-install.sh --version latest --channel 7.0 --runtime dotnet
    echo 'export DOTNET_ROOT="$HOME/.dotnet"' >> $HOME/.bashrc
    echo 'export PATH="$PATH:$HOME/.dotnet"' >> $HOME/.bashrc
    echo 'export PATH="$PATH:$HOME/.dotnet/tools"' >> $HOME/.bashrc
    ```

    After changing the .bashrc file, you'll need to restart your terminal to see the changes.

2. Setup credential cache

    See [GCM Credential stores](https://github.com/git-ecosystem/git-credential-manager/blob/main/docs/credstores.md) for more info and additional options.

    ```bash
    git config --global credential.credentialStore cache
    ```

3. Install\Configure GMC (Git Credential Manager)

    ```bash
    dotnet tool install git-credential-manager
    git-credential-manager configure
    ```

4. Install Prerequisites

    Install the following packages via `apt-get` to enable building of the pico-sdk-js package along with submodules [jerryscript](https://github.com/jerryscript-project/jerryscript/blob/master/docs/00.GETTING-STARTED.md) and [pico-sdk](TBD)

    ```bash
    sudo apt-get install -y nodejs npm cmake gcc gcc-arm-none-eabi cppcheck clang-format libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
    ```

    *Note: The jerryscript instructions include installing `python`, but this is already installed by default in Raspberry Pi OS and will cause an error to show up in apt-get due to a conflict. Additionally, jerryscript includes installing clang-format-10, but this is no longer available. Installing the core clang-format (currently version 14) seems to work just fine.*

## Building locally

1. Clone the repo

    ```git clone https://github.com/jt000/pico-sdk-js.git```

    Note: If this is the first time you've cloned, then you'll get a popup to enter your credentials. If you get prompted to enter your github credentials in the terminal, then GCM was not properly installed or configured.

    Note: If you get an gpg error saying "public key not found", then you did not configure your credential cache correctly.

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
