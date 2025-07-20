# Field Map Editor

[![Build status](https://ci.appveyor.com/api/projects/status/v68sdv6ldtv3ll22?svg=true)](https://ci.appveyor.com/project/Sebanisu/field-map-editor) 
[![C++ 23](https://img.shields.io/badge/C++%20-23-ff69b4.svg)](https://en.cppreference.com/w/cpp/23) 
<!--![CMake](https://github.com/Sebanisu/Field-Map-Editor/workflows/CMake/badge.svg)-->

Test Application for editing the tiles of a FF8 field. See [releases](https://github.com/Sebanisu/Field-Map-Editor/releases) to download the most recent build. Everything is in constant flux. You might want to backup your current build of the program before upgrading as their maybe breaking changes.

**Possible Issue:** with saving and loading of filters. If you have problems, clear out the `.toml` file or the `imgui.ini` to reset the settings to defaults. If you want to send me your `.toml` in an issue. I might be able to figure out why it was bugging out.

#### Pupu Compatibility Issue

-  **2025.JAN.13** - I found a small bug https://github.com/Sebanisu/Field-Map-Editor/issues/121. This could cause the Pupu to not be unique enough. It might break your exports. You may need to convert to swizzled in your current version before upgrading.
-  **2022.SEP.12** - https://github.com/Sebanisu/Field-Map-Editor/issues/80 & https://github.com/Sebanisu/Field-Map-Editor/issues/122. I used 2 bits to note if a tile is not aligned with the 16x16 grid, via the X-axis or the Y-axis.

---

### Main Application

This was formerly the **SFML version**, but it now uses **OpenGL** with GLEW, GLFW, and ImGui, just like the Experimental Application. It is the more feature-complete version and is recommended for most users.

It receives regular updates, as it’s the version most people use. New features are developed and tested here first to ensure stability and reliability.

If you notice any missing features or have suggestions, feel free to share your feedback — it directly shapes development.

> **Note:** This version requires the `fonts` folder.

> **Note:** This version also requires the `res` folder, which contains essential shaders and textures.

![image](https://github.com/user-attachments/assets/730fbd48-b742-47cf-bffa-b7a106e586b6)

#### Batch Operation Window

The Batch Operation Window will let you take input in one format and output in another format. Input formats such as `.mim`, swizzle, and deswizzle. Output formats such as swizzle, deswizzle, and swizzle as one file.

![image](https://github.com/user-attachments/assets/d67f25ba-b6a8-4efc-b724-17bb98da315c)

#### Impot Window WIP

This feature needs more work. I did have a working test where I used this to add tiles to the swizzle, loaded in the game. You might still be able to get it to work.

![image](https://github.com/user-attachments/assets/73d254c3-0835-4b1f-a93d-43ddeb020deb)

#### History Window

The History Window lets you see opreations performed that you can undo or redo.

![image](https://github.com/user-attachments/assets/b6da167d-6627-4b36-841c-9c4b12abe76f)

#### Draw and Control Panel Window

The Draw window shows the current rendering of the field. The Control Panel controls the Draw window. You can also access more detailed configuration in the main menu at the top.

![image](https://github.com/user-attachments/assets/322c7018-ab12-42fd-8a92-356585a93356)

#### Custom Paths Window

The Custom Paths Window lets you configure how the Field Map Editor search for 3rd party textures, or `.map` files.

![image](https://github.com/user-attachments/assets/08fa64bd-312d-4304-b999-4f28440b191a)

#### Field File Window WIP

This window lets you see what files are in this field. I plan to make this alittle more useful. You can copy and paste lines of the text.

![image](https://github.com/user-attachments/assets/f9b2ddfd-3c72-4ea6-a658-62ab596aa2ce)


---

### Experimental Application

This was formerly the **OpenGL version** and now serves as a testing ground for experimental features. Like the Main Application, it also uses **OpenGL**, GLEW, GLFW, and ImGui.

Experimental features are often trialed here first, then refined and ported to the Main Application if successful. While the Main Application is the recommended choice, the Experimental Application may include features not yet available there.

If you find something in the Experimental version that you'd like to see in the Main Application, let me know — your feedback helps guide future development.

> **Note:** This version requires the `res` folder, which contains essential shaders and textures.

![image](https://github.com/user-attachments/assets/57f33eb5-83f1-4ed1-9d0a-c19678651305)


---


### Credits

This project is based on the work of the following developers:

- [Omzy](https://forums.qhimm.com/index.php?topic=13444.0) ([source](https://github.com/Sebanisu/Pupu))
- [myst6re](https://forums.qhimm.com/index.php?topic=13050.0) ([source](https://github.com/myst6re/deling))
- [Maki](https://forums.qhimm.com/index.php?topic=18656.0) ([source](https://github.com/MaKiPL/OpenVIII-monogame))
- [TheCherno](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT) ([source](https://github.com/thecherno))

I referenced their code or videos in the development of this project.

---

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


---

![Running on Arch Linux](https://github.com/user-attachments/assets/7dd33451-1696-49d0-8856-12ac2e608145)

# Linux Build Instructions **(WIP)**
- Make sure you have atleast GCC 15.
```sh
gcc --version
```

### Step 1. Update and install required packages

Ubuntu
```sh
sudo apt update
sudo apt install -y python3 python3-venv python3-pip cmake build-essential
```p

Arch
```sh
pacman -Syu sudo
sudo pacman -Syu
sudo pacman -Syu --needed python python-virtualenv python-pip cmake base-devel python-pipx libglvnd libxcb libfontenc libice libsm libxaw libxcomposite libxcursor libxdamage libxtst libxinerama libxkbfile libxrandr libxres libxss libxv xcb-util xcb-util-cursor xcb-util-wm xcb-util-keysyms git less
```

### Step 2. Create and activate a Python virtual environment

Ubuntu
```sh
python3 -m venv venv
source venv/bin/activate
```


### Step 3. Install Conan package manager

```sh
pip install conan
```
Arch
```sh
pipx install conan
pipx ensurepath
source ~/.bashrc
```

### Step 3.5 Copy dev files to your home directory.
I was getting permission errors in wsl. So I copied my files to my home directory to try again. I donno why it was happening. Below I am making the projects folder and i'm removing existing files and i'm cloning the files. The remove step is optional. I had old files in that directory.
```sh
mkdir -p ~/projects
rm -rf ~/projects/Field-Map-Editor
git clone --branch remove_sfml https://github.com/Sebanisu/Field-Map-Editor.git ~/projects/Field-Map-Editor
cd ~/projects/Field-Map-Editor
git status
git pull
```


### Step 4. Configure and Install dependencies using Conan
#### Set Conan's home directory (optional, useful for CI environments)

```sh
conan config home
```

#### Detect a Conan profile and name it `ubuntu24` if it doesn't already exist
#### Replace `ubuntu24` with the appropriate profile name if necessary
#### Redirect stdout and stderr to /dev/null and ensure the script continues regardless of errors

Ubuntu
```sh
conan profile detect --name ubuntu24 > /dev/null 2>&1 || true
```

Arch
```sh
conan profile detect --name arch > /dev/null 2>&1 || true
```

#### List all available Conan profiles
```sh
conan profile list
```
#### Install dependencies using Conan

Ubuntu
```sh
conan install . -pr ubuntu24 -pr:b ubuntu24 --build=missing -s compiler.cppstd=23 -of ./linux
```

Arch
```sh
conan install . -pr arch -pr:b arch --build=missing -s compiler.cppstd=23 -of ./archlinux -c tools.system.package_manager:mode=install
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
rm -rf ./archlinux/build/Release/
cmake --preset=conan_linux_release-release
```

### Step 8. Build the project
```sh
cmake --build ./archlinux/build/Release/
```

## Additional Notes
- The compiled binaries can be found in the `./bin/` directory.
