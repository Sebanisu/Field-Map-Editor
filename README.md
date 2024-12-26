# Field Map Editor

[![Build status](https://ci.appveyor.com/api/projects/status/v68sdv6ldtv3ll22?svg=true)](https://ci.appveyor.com/project/Sebanisu/field-map-editor)
<!--![CMake](https://github.com/Sebanisu/Field-Map-Editor/workflows/CMake/badge.svg)-->

Test Application for editing the tiles of a FF8 field. See [releases](https://github.com/Sebanisu/Field-Map-Editor/releases) to download the most recent build. Everything is in constant flux.

### SFML Version

The SFML version is the more feature-complete version and is recommended for most users. Recently, it has seen active updates because it is the version most people are using. New features are now being developed and implemented in the SFML version first, ensuring it stays up to date and reliable.

If you find any features missing from the SFML version or have suggestions, please let me know. Your feedback helps shape its development.

### OPENGL Version

The OPENGL version has evolved into a testing ground for new features. These features are tested and refined in the OPENGL version and then backported to the SFML version where applicable. While the SFML version is the recommended version for most users, the OPENGL version might contain experimental features that have not yet made their way to the SFML version.

If you are using the OPENGL version and find a feature you would like to see in the SFML version, please let me know. Your feedback helps prioritize what gets implemented in the SFML version.

**Note:** The OPENGL version requires the following:
1. Download the `res` folder from [this location](https://github.com/Sebanisu/Field-Map-Editor/tree/main/src/opengl/opengl_version/res).
2. Alternatively, the `res.zip` file is included in the [releases](https://github.com/Sebanisu/Field-Map-Editor/releases). Extract it to the same directory as the `.exe` file.

This folder contains the necessary shaders and textures.

### Credits

This project is based on the work of [Omzy](https://forums.qhimm.com/index.php?topic=13444.0) [(src)](https://github.com/Sebanisu/Pupu) and [myst6re](https://forums.qhimm.com/index.php?topic=13050.0) [(src)](https://github.com/myst6re/deling). Also [Maki](https://forums.qhimm.com/index.php?topic=18656.0) [(src)](https://github.com/MaKiPL/OpenVIII-monogame). I referenced their code to make this.

### Pupu Compatablility Warning!
* I updated my Pupu ID generation to use 2 bits that were unused. These tell if the X coordinate and/or the Y coordinate aren't aligned with the 16 x 16 grid. This was because a few instances where tiles weren't being deswizzled or swizzled correctly. And you'd end up with two layers of tiles merged together. This change happened around September 2022 so if you already created deswizzles the filenames won't match anymore.


### Build instructions
* install vs2022
* choco install conan
* conan config home
* conan profile detect --name windows10_msvc
* edit your profile .conan2/profiles/windows10_msvc
* conan install . -pr windows10_msvc -pr:b windows10_msvc  --build=missing
* cmake --version
* cmake --list-presets
* cmake --preset conan_windows-default
* rmdir /S /Q build


### linux

* sudo apt update
* sudo apt install python3 python3-venv python3-pip cmake
* source venv/bin/activate
* pip install conan
or
* pip install -r requirements.txt
* deactivate
* conan install . -pr ubuntu24 -pr:b ubuntu24 --build=missing -of ./linux
* cmake --list-presets
* cmake --preset=conan_linux-relwithdebinfo
* cd ./linux/build/RelWithDebInfo/
* make
* cd ../../..
* rm -r ./linux
