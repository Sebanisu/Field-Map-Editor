# Field Map Editor

[![Build status](https://ci.appveyor.com/api/projects/status/v68sdv6ldtv3ll22?svg=true)](https://ci.appveyor.com/project/Sebanisu/field-map-editor)
<!--![CMake](https://github.com/Sebanisu/Field-Map-Editor/workflows/CMake/badge.svg)-->

Test Application for editing the tiles of a FF8 field. See [releases](https://github.com/Sebanisu/Field-Map-Editor/releases) to download the most recent build. Everything is in constant flux.
* SFML version is more feature complete. I have been updating this recently because people have been using it. So now new features are getting put into this first.
* OPENGL version to have more control over what the API is doing. I'm slowly trying to move features to this version but it's lagging behind. Goal was to eventually stop using the SFML version in the future.
  * This version requires the https://github.com/Sebanisu/Field-Map-Editor/tree/main/src/opengl/opengl_version/res folder to be placed in the same directory as the `.exe` file. That folder contains the shaders and textures.

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
* cmake --preset conan-relwithdebinfo
linux
* sudo apt update
* sudo apt install python3 python3-venv python3-pip
* source venv/bin/activate
* pip install conan
or
* pip install -r requirements.txt
* deactivate
* conan install . -pr ubuntu24 -pr:b ubuntu24 --build=missing -of ./linux