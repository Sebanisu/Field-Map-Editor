# Field Map Editor

[![Build status](https://ci.appveyor.com/api/projects/status/v68sdv6ldtv3ll22?svg=true)](https://ci.appveyor.com/project/Sebanisu/field-map-editor)
<!--![CMake](https://github.com/Sebanisu/Field-Map-Editor/workflows/CMake/badge.svg)-->

Test Application for editing the tiles of a FF8 field. See [releases](https://github.com/Sebanisu/Field-Map-Editor/releases) to download the most recent build. Everything is in constant flux. You might want to backup your current build of the program before upgrading as their maybe breaking changes.

#### Pupu Compatibility Issue

-  **2025.JAN.13** - I found a small bug https://github.com/Sebanisu/Field-Map-Editor/issues/121. This could cause the Pupu to not be unique enough. It might break your exports. You may need to convert to swizzled in your current version before upgrading.
-  **2022.SEP.12** - https://github.com/Sebanisu/Field-Map-Editor/issues/80 & https://github.com/Sebanisu/Field-Map-Editor/issues/122. I used 2 bits to note if a tile is not aligned with the 16x16 grid, via the X-axis or the Y-axis.

### SFML Version

The SFML version is the more feature-complete version and is recommended for most users. Recently, it has seen active updates because it is the version most people are using. New features are now being developed and implemented in the SFML version first, ensuring it stays up to date and reliable.

If you find any features missing from the SFML version or have suggestions, please let me know. Your feedback helps shape its development.

**Note:** The SFML version requires the `fonts` folder.

![image](https://github.com/user-attachments/assets/730fbd48-b742-47cf-bffa-b7a106e586b6)


### OPENGL Version

The OPENGL version has evolved into a testing ground for new features. These features are tested and refined in the OPENGL version and then backported to the SFML version where applicable. While the SFML version is the recommended version for most users, the OPENGL version might contain experimental features that have not yet made their way to the SFML version.

If you are using the OPENGL version and find a feature you would like to see in the SFML version, please let me know. Your feedback helps prioritize what gets implemented in the SFML version.

**Note:** The OPENGL version requires the `res` folder, which contains the necessary shaders and textures.

![image](https://github.com/user-attachments/assets/57f33eb5-83f1-4ed1-9d0a-c19678651305)


### Credits

This project is based on the work of [Omzy](https://forums.qhimm.com/index.php?topic=13444.0) [(src)](https://github.com/Sebanisu/Pupu) and [myst6re](https://forums.qhimm.com/index.php?topic=13050.0) [(src)](https://github.com/myst6re/deling). Also [Maki](https://forums.qhimm.com/index.php?topic=18656.0) [(src)](https://github.com/MaKiPL/OpenVIII-monogame). I referenced their code to make this.

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
conan install . -pr windows_msvc -pr:b windows_msvc --build=missing -s compiler.cppstd=23 -s build_type=Release -of build/Release
conan install . -pr windows_msvc -pr:b windows_msvc --build=missing -s compiler.cppstd=23 -s build_type=RelWithDebInfo -of build/RelWithDebInfo
conan install . -pr windows_msvc -pr:b windows_msvc --build=missing -s compiler.cppstd=23 -s build_type=Debug -of build/Debug
deactivate
```

### Step 3: Configure and Build with CMake

Check the CMake version and list available presets:

```cmd
cmake --version
cmake --list-presets
```

Configure and generate build files using the preset: (Conan loves to add default to the preset.)

```cmd
cmake --preset conan_windows-release-default
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

### Step 1. Update and install required packages

```sh
sudo apt update
sudo apt install -y python3 python3-venv python3-pip cmake build-essential
```

### Step 2. Create and activate a Python virtual environment
```sh
python3 -m venv venv
source venv/bin/activate
```

### Step 3. Install Conan package manager

```sh
pip install conan
```

### Step 4. Configure and Install dependencies using Conan
#### Set Conan's home directory (optional, useful for CI environments)
```sh
conan config home
```

#### Detect a Conan profile and name it `ubuntu24` if it doesn't already exist
#### Replace `ubuntu24` with the appropriate profile name if necessary
#### Redirect stdout and stderr to /dev/null and ensure the script continues regardless of errors
```sh
conan profile detect --name ubuntu24 > /dev/null 2>&1 || true
```

#### List all available Conan profiles
```sh
conan profile list
```
#### Install dependencies using Conan
```sh
conan install . -pr ubuntu24 -pr:b ubuntu24 --build=missing -of ./linux
```

### Step 5. Deactivate the virtual environment
```sh
deactivate
```

### Step 6. Check available CMake presets (optional)
```sh
cmake --list-presets
```

### Step 7. Configure the build with CMake
```sh
cmake --preset=conan_linux-relwithdebinfo
```

### Step 8. Build the project
```sh
cd ./linux/build/RelWithDebInfo/
make
```

### Step 9. Return to the root directory (optional)
```sh
cd ../../../
```
## Additional Notes
- The compiled binaries can be found in the `./linux/build/RelWithDebInfo/` directory.

