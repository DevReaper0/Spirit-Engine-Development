# Spirit Engine ![License](https://img.shields.io/github/license/DaRubyMiner360/Spirit-Engine-Development.svg)

<!-- **THIS PAGE IS THE DEVELOPMENT VERSION!!
TO GET THE NORMAL RELEASES, USE THIS LINK:
[Spirit Engine](https://github.com/DaRubyMiner360/Spirit-Engine).
JUST REMEMBER THAT THE THIS MIGHT HAVE MANY MORE BUGS THAN THE NORMAL RELEASES!!
DON'T SAY I DIDN'T WARN YOU!** -->

![Spirit Engine](/Resources/Branding/Spirit_Logo_Text_Light_Square.png?raw=true "Spirit Engine")

Spirit Engine is primarily an early-stage interactive application and rendering engine for Windows, Mac, and Linux. Currently not much is implemented.

## Supported Platforms

Spirit Engine currently supports:

- Computer OS:
  - ![Windows Supported](https://img.shields.io/badge/Windows-win--64-green.svg)
  - ![Linux Supported](https://img.shields.io/badge/Linux-Arch%20%7C%20Debian-green.svg)
  - ![MacOS Supported](https://img.shields.io/badge/MacOS-Supported-green.svg)
- Mobile OS:
  - ![Android Not Supported](https://img.shields.io/badge/Android-Not%20Supported-red.svg)
  - ![IOS Not Supported](https://img.shields.io/badge/IOS-Not%20Supported-red.svg)

Windows, Mac, and Linux are currently supported with plans for Android/IOS support in the future.

## Getting Started

Visual Studio 2017 or 2019 is recommended, Spirit Engine is officially untested on other development environments whilst I focus on a Windows build.

You can clone the repository to a local destination using git:

`git clone --recursive https://github.com/DaRubyMiner360/Spirit-Engine-Development`

Make sure that you do a `--recursive` clone to fetch all of the submodules!

### Windows

Premake is provided as [premake5.exe](https://github.com/DaRubyMiner360/Spirit-Engine-Development/blob/master/vendor/bin/premake/premake5.exe) in the repository. Execute and follow the install instructions.

### Linux

Premake is provided as [premake5-linux.tar.gz](https://github.com/DaRubyMiner360/Spirit-Engine-Development/blob/master/vendor/bin/premake/premake5-linux.tar.gz) in the repository. To download and install it, follow the instructions:

```
$ wget https://github.com/DaRubyMiner360/Spirit-Engine-Development/raw/master/vendor/bin/premake/premake5-linux.tar.gz
$ tar -xzvf premake5-linux.tar.gz
$chmod +x premake5-linux # make premake executable
$ sudo cp premake5-linux /usr/bin/

$ premake5 --help
```

Spirit Engine has extra development dependencies needed for Linux. The following packages are needed to compile the project:

- `libxcursor`
- `libxrandr`
- `libxinerama`
- `libxi`

#### Arch

On Arch and Arch derivative distributions, install these packages by running:

`sudo pacman -S libxcursor libxinerama libxi`

#### Debian

On Debian and Debian derivative distributions, install these packages by running:

`sudo apt install libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libglu1-mesa-dev`

Spirit Engine is configured and compiled with:

```bash
premake5 gmake2
make
```

## The Plan

The plan for Spirit Engine is two-fold: to create a powerful 2D and 3D engine.

### Main features to come:

- Fast 2D rendering (UI, particles, sprites, etc.)
- High-fidelity Physically-Based 3D rendering (this will be expanded later, 2D to come first)
- Support for Mac, Linux, Android and iOS
  - Native rendering API support (DirectX, Vulkan, Metal)
- Fully featured viewer and editor applications
- Fully scripted interaction and behavior
- Integrated 3rd party 2D and 3D physics engine
- Procedural terrain and world generation
- Artificial Intelligence
<!-- - An official IDE that can be used in the engine -->

## Short term goals :

<!-- _Note: this is subject to change at any time! Follow the roadmap over at [hazelengine.com/roadmap](http://hazelengine.com/roadmap)._ -->

By the end 2020, I want to make a game using Spirit Engine. Not like the time I made a game in one hour using the engine, but this time by using the proper tools that would be required to make a game with Spirit Engine. This means we need to add a full 2D workflow:

- Design the game scene by using Hazelnut, the Spirit Engine editor,
- Test the game inside Spirit Editor, including the ability to save/load the created game,
- Load and play the game inside Sandbox.

We want everyone to be able to play the game on all desktop platforms (Windows, Mac and Linux). When this is implemented, another attempt at the "Creating a game in one hour using Spirit Engine" will be made to see how far the engine has become.

[![Twitter](https://img.shields.io/badge/%40darubyminer360--blue.svg?style=social&logo=Twitter)](https://twitter.com/darubyminer360)

<!-- [![Instagram](https://img.shields.io/badge/thecherno--red.svg?style=social&logo=Instagram)](https://www.instagram.com/thecherno) -->

[![Youtube](https://img.shields.io/badge/TheChernoProject--red.svg?style=social&logo=youtube)](http://youtube.com/channel/UCJdZupuPgueQTLY2cg4L7yw/)

<!-- [![Discord](https://img.shields.io/badge/DaRubyMiner360%20Server--blue.svg?style=social&logo=Discord)](https://discord.gg/K2eSyQA) -->

<!-- [![Patreon](https://img.shields.io/badge/%40thecherno--green.svg?style=social&logo=Patreon)](https://patreon.com/thecherno) -->
