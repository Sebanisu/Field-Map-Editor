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

**Note:** The OPENGL version requires the `res` folder, which contains the necessary shaders and textures.:
1. Download the `res` folder from [this location](https://github.com/Sebanisu/Field-Map-Editor/tree/main/src/opengl/opengl_version/res).
2. Alternatively, the `res.zip` file is included in the [releases](https://github.com/Sebanisu/Field-Map-Editor/releases). Extract it to the same directory as the `.exe` file.



### Credits

This project is based on the work of [Omzy](https://forums.qhimm.com/index.php?topic=13444.0) [(src)](https://github.com/Sebanisu/Pupu) and [myst6re](https://forums.qhimm.com/index.php?topic=13050.0) [(src)](https://github.com/myst6re/deling). Also [Maki](https://forums.qhimm.com/index.php?topic=18656.0) [(src)](https://github.com/MaKiPL/OpenVIII-monogame). I referenced their code to make this.

### Pupu Compatablility Warning!
* I updated my Pupu ID generation to use 2 bits that were unused. These tell if the X coordinate and/or the Y coordinate aren't aligned with the 16 x 16 grid. This was because a few instances where tiles weren't being deswizzled or swizzled correctly. And you'd end up with two layers of tiles merged together. This change happened around September 2022 so if you already created deswizzles the filenames won't match anymore.


# Windows Build Instructions

This guide provides detailed instructions for building the project on Windows using the provided build scripts and dependencies.

## Prerequisites

Ensure the following tools and software are installed on your system:

- **Microsoft Visual Studio 2022** (Community Edition or higher)
- **Python 3.8 or newer**
- **Conan 2.x or newer**
- **CMake**

## Build Steps

### Step 1: Setup Environment

Run the Visual Studio `vcvars64.bat` script to configure the environment variables for 64-bit builds:

```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

### Step 2: Install Conan Dependencies

Ensure Conan 2.x or newer is installed and set up:

```cmd
python -m venv .\venv
.\venv\Scripts\activate
python -m pip install --user "conan>=2.0" --upgrade
```

Configure Conan and detect profiles:

```cmd
conan config home
conan profile detect --name windows_msvc > nul 2> nul & exit 0
conan profile list
```

Install project dependencies with Conan:

```cmd
conan install . -pr windows_msvc -pr:b windows_msvc --build=missing -s compiler.cppstd=23
deactivate
```

### Step 3: Configure and Build with CMake

Check the CMake version and list available presets:

```cmd
cmake --version
cmake --list-presets
```

Configure and generate build files using the preset:

```cmd
cmake --preset conan_windows-default
```

### Step 4: Build the Project

Navigate to the build directory and build the solution:

```cmd
cd build
msbuild Field-Map-Editor.sln /p:Configuration=Release
cd ..
dir /s /b bin\*.exe
```

## Additional Notes

- The resulting executables will be located in the `bin` directory.
- Ensure all paths and configurations match your local setup.
- For troubleshooting, review the output logs during each step.



# Linux Build Instructions **(WIP)**

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
