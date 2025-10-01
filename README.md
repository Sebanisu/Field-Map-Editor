# Field Map Editor

[![Windows Build](https://github.com/Sebanisu/Field-Map-Editor/actions/workflows/main-1.0.0.yaml/badge.svg)](https://github.com/Sebanisu/Field-Map-Editor/actions/workflows/main-1.0.0.yaml)
[![Linux Build](https://github.com/Sebanisu/Field-Map-Editor/actions/workflows/ubuntu.yaml/badge.svg)](https://github.com/Sebanisu/Field-Map-Editor/actions/workflows/ubuntu.yaml)
[![C++ 23](https://img.shields.io/badge/C++%20-23-ff69b4.svg)](https://en.cppreference.com/w/cpp/23) 
[![Contributions Welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/Sebanisu/Field-Map-Editor/pulls)
<!--![CMake](https://github.com/Sebanisu/Field-Map-Editor/workflows/CMake/badge.svg)-->

Test Application for editing the tiles of a FF8 field. See [releases](https://github.com/Sebanisu/Field-Map-Editor/releases) to download the most recent build. Everything is in constant flux. You might want to backup your current build of the program before upgrading as their maybe breaking changes.

**Possible Issue:** with saving and loading of filters. If you have problems, clear out the `.toml` file or the `imgui.ini` to reset the settings to defaults. If you want to send me your `.toml` in an issue. I might be able to figure out why it was bugging out.

#### Pupu Compatibility Notes
The original Pupu application generates IDs differently. I’ve tried to stay mostly consistent, but there are a few differences to be aware of:

- **2025.JAN.13** — [Issue #121](https://github.com/Sebanisu/Field-Map-Editor/issues/121)  
  Fixed a bug that could cause Pupu IDs to be insufficiently unique, potentially breaking exports.  
  If you are upgrading, you may need to convert your data to the swizzled format in your current version first.

- **2022.SEP.12** — [Issue #80](https://github.com/Sebanisu/Field-Map-Editor/issues/80), [Issue #122](https://github.com/Sebanisu/Field-Map-Editor/issues/122)  
  I used 2 bits to record whether a tile is misaligned with the 16×16 grid along the X-axis or Y-axis.


---

## Graphics Drivers

This project makes use of GPU rendering. Outdated or broken graphics drivers may cause crashes, display issues, or poor performance.  
If you run into problems, **please update your graphics drivers** before reporting issues.

### Tested Hardware
- **NVIDIA GeForce GTX 980 Ti** (released in 2015–2016 era)  
  Works reliably. Other GPUs from NVIDIA or AMD from around the same time or newer should also work.  
- Intel integrated graphics: one user reported issues. I cannot fully test this at the moment, so please ensure you are on the latest drivers.

### Driver Update Links
- **Intel**
  - [Intel Arc, Iris Xe, and 11th+ Gen Graphics](https://www.intel.com/content/www/us/en/download/785597/intel-arc-iris-xe-graphics-windows.html)  
    (May also cover integrated graphics on 11th–14th Gen CPUs)  
  - [Intel 11th–14th Gen Processor Graphics](https://www.intel.com/content/www/us/en/download/864990/intel-11th-14th-gen-processor-graphics.html)  
  - [Intel 7th–10th Gen Processor Graphics](https://www.intel.com/content/www/us/en/download/776137/intel-7th-10th-gen-processor-graphics-windows.html)  
  - [Intel 6th Gen Processor Graphics](https://www.intel.com/content/www/us/en/download/762755/intel-6th-10th-gen-processor-graphics-windows.html)  
  - If you’re unsure which driver to install, use the  
    [Intel Driver & Support Assistant](https://www.intel.com/content/www/us/en/support/detect.html)

- **NVIDIA**  
  - [GeForce Drivers](https://www.nvidia.com/download/index.aspx)

- **AMD**  
  - [Radeon Drivers](https://www.amd.com/en/support)

---

⚠️ If you experience graphical issues, **always update your drivers first** before assuming it’s a problem with this software.


### Main Application

This was formerly the **SFML version**, but it now uses **OpenGL** with GLEW, GLFW, and ImGui, just like the Experimental Application. It is the more feature-complete version and is recommended for most users.

It receives regular updates, as it’s the version most people use. New features are developed and tested here first to ensure stability and reliability.

If you notice any missing features or have suggestions, feel free to share your feedback — it directly shapes development.

> **Note:** This version requires the `fonts` folder.

> **Note:** This version also requires the `res` folder, which contains essential shaders and textures.

![image](https://github.com/user-attachments/assets/730fbd48-b742-47cf-bffa-b7a106e586b6)

#### Hotkeys

I added a help menu that lists out keyboard shortcuts.

![image](https://github.com/user-attachments/assets/860c9daf-70da-4dc2-be5d-fb6bdbd813d2)


#### Batch Operation Window

The **Batch Operation Window** allows you to take input in one format and output it in another.  

---

**Supported Input Formats:**

- **`.mim`** — Mim is the storage format of the field textures in *Final Fantasy VIII*. Selecting this option means you are using the original low-resolution textures.  
- **Deswizzle** — Load a directory of deswizzled textures, including `.map` files if present.  
- **Swizzle** — Load a directory of swizzled textures, including `.map` files if present.  
- **Swizzle as One Image** — Load a directory of swizzled “as one image” textures, including `.map` files if present.  
  - ⚠️ May not load correctly without a `.map` file. Might require enforcing FFNX formatting when `.map` is missing.  
- **Full Filename Image** — Load a directory of full filename textures, including `.map` files if present (`.toml` supported).  
  - Uses the **`Deswizzle.toml`** configuration file to read filenames and **PupuIDs**.  

---

**Supported Output Formats:**

- **Deswizzle** — Output in deswizzled format.  
- **Swizzle** — Output in swizzled format.  
- **Swizzle as One Image** — Output swizzled textures combined into a single image.  
- **Deswizzle + Generate TOML** — Output deswizzled textures and generate a new `deswizzle.toml` configuration.  
- **Deswizzle (Full Filename)** — Output deswizzled textures using the full filename convention (based on `deswizzle.toml`).  
- **CSV** — Outputs a `.csv` file containing a dump of all the tiles found in the `.map` file.  

---

![image](https://github.com/user-attachments/assets/d67f25ba-b6a8-4efc-b724-17bb98da315c)  

#### Deswizzle.toml Editor ***(NEW)***

The **Deswizzle.toml Editor** provides a configuration interface for defining image combinations used during batch generation.  

- **PupuIDs** are the primary keys used to group images together and later separate them back into individual components.  
- **Filters**:  
  - Filters can be applied to **output images only**.  
  - Input images do **not** support filters other than PupuIDs.  
- **Masks** (batch output only):  
  - **Multi-colored masks** can be generated to define cut regions.  
  - **White-on-black masks** can also be generated for external editing tools (e.g., Photoshop).  
  - White-on-black masks are **output-only** and are **not accepted as input** for batch operations.  

You may edit the **`deswizzle.toml`** file directly to customize configurations.  
- Configurations can be **shared with others** or contributed back via a [pull request](https://github.com/Sebanisu/Field-Map-Editor/pulls).  
- The current version of `deswizzle.toml` is **preliminary** and may contain bugs; it was created in limited time for the initial release.  

> ⚠️ **Note:** The `deswizzle.toml` included with this release is an **initial draft**.  
> Expect incomplete mappings or errors. Contributions and improvements are welcome.  

![image](https://github.com/user-attachments/assets/8305b9a7-5f3e-4f5a-9cae-c2667dc610ac)  
![image](https://github.com/user-attachments/assets/12d567f5-ec35-42b9-a489-4e7c3590b4e9)  


#### Impot Window WIP

Feature is **disabled** for now needs reworked!
This feature needs more work. I did have a working test where I used this to add tiles to the swizzle, loaded in the game. You might still be able to get it to work in an older build of this program.

![image](https://github.com/user-attachments/assets/73d254c3-0835-4b1f-a93d-43ddeb020deb)

#### History Window

The History Window lets you see opreations performed that you can undo or redo. The undo and redo only effect the edits to the .map currently loaded in memory.

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

This project is based on the work of the following developers. I referenced their code or videos in the development of this project:

| Developer | Forum / Profile | Source |
|-----------|----------------|--------|
| Omzy      | [Link](https://forums.qhimm.com/index.php?topic=13444.0) | [GitHub](https://github.com/Sebanisu/Pupu) |
| myst6re  | [Link](https://forums.qhimm.com/index.php?topic=13050.0) | [GitHub](https://github.com/myst6re/deling) |
| Maki      | [Link](https://forums.qhimm.com/index.php?topic=18656.0) | [GitHub](https://github.com/MaKiPL/OpenVIII-monogame) |
| TheCherno | [Link](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT) | [GitHub](https://github.com/thecherno) |
| TrueOdin  | [Link](https://forums.qhimm.com/index.php?topic=19970.0) | [GitHub](https://github.com/julianxhokaxhiu/FFNx) |


### Build Info

[![OpenGL 4.3](https://img.shields.io/badge/OpenGL-4.3-blue.svg)](https://www.khronos.org/opengl/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-lightgrey.svg)](https://en.cppreference.com/w/cpp/23)
[![Conan](https://img.shields.io/badge/dependencies-conan-green.svg)](https://conan.io/)

#### Libraries (via Conan)
[![lz4 1.10.0](https://img.shields.io/badge/lz4-1.10.0-blue.svg)](https://github.com/lz4/lz4)
[![spdlog 1.15.0](https://img.shields.io/badge/spdlog-1.15.0-blue.svg)](https://github.com/gabime/spdlog)
[![glfw 3.4](https://img.shields.io/badge/glfw-3.4-blue.svg)](https://www.glfw.org/)
[![glew 2.2.0](https://img.shields.io/badge/glew-2.2.0-blue.svg)](http://glew.sourceforge.net/)
[![glm 0.9.9.8](https://img.shields.io/badge/glm-0.9.9.8-blue.svg)](https://github.com/g-truc/glm)
[![imgui 1.91.8-docking](https://img.shields.io/badge/imgui-1.91.8--docking-blue.svg)](https://github.com/ocornut/imgui)
[![boost.ut 2.1.0](https://img.shields.io/badge/boost.ut-2.1.0-blue.svg)](https://github.com/boost-ext/ut)
[![toml++ 3.0.1](https://img.shields.io/badge/toml++-3.0.1-blue.svg)](https://github.com/marzer/tomlplusplus)
[![libpng 1.6.44](https://img.shields.io/badge/libpng-1.6.44-blue.svg)](http://www.libpng.org/pub/png/libpng.html)
[![stb](https://img.shields.io/badge/stb-cci.20230920-blue.svg)](https://github.com/nothings/stb)
[![iconfontcppheaders](https://img.shields.io/badge/iconfontcppheaders-cci.20240620-blue.svg)](https://github.com/owengage/iconfontcppheaders)
[![ctre 3.9.0](https://img.shields.io/badge/ctre-3.9.0-blue.svg)](https://github.com/eranpeer/CompileTimeRegex)

#### Libraries (via CMake FetchContent)
[![imgui-filebrowser fork](https://img.shields.io/badge/imgui--filebrowser-fork-blue.svg)](https://github.com/yourusername/imgui-filebrowser)
[![ImGuizmo eb90849](https://img.shields.io/badge/ImGuizmo-eb90849-blue.svg)](https://github.com/CedricGuillemet/ImGuizmo/tree/eb90849)
[![OpenVIII_CPP_WIP](https://img.shields.io/badge/OpenVIII_CPP_WIP-latest-blue.svg)](https://github.com/Sebanisu/OpenVIII_CPP_WIP)
[![ToolsLibrary](https://img.shields.io/badge/ToolsLibrary-latest-blue.svg)](https://github.com/Sebanisu/ToolsLibrary)

#### Build Tools
[![CMake 3.31.6](https://img.shields.io/badge/CMake-3.31.6-blue.svg)](https://cmake.org/)
[![Ninja 1.12.1](https://img.shields.io/badge/Ninja-1.12.1-blue.svg)](https://ninja-build.org/)
[![Visual Studio 2022 v17.14](https://img.shields.io/badge/Visual%20Studio-2022%20v17.14-blue.svg)](https://visualstudio.microsoft.com/vs/)
[![GCC 14.2.0](https://img.shields.io/badge/GCC-14.2.0-red.svg)](https://gcc.gnu.org/)

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
- Make sure you have atleast GCC 14.2.
```sh
gcc --version
```

### Step 1. Update and install required packages
- Note: some of these might not be needed anymore I just keep adding as I need do.

Arch
```sh
pacman -Syu sudo
sudo pacman -Syu
sudo pacman -Syu --needed python python-virtualenv python-pip cmake base-devel python-pipx libglvnd libxcb libfontenc libice libsm libxaw libxcomposite libxcursor libxdamage libxtst libxinerama libxkbfile libxrandr libxres libxss libxv xcb-util xcb-util-cursor xcb-util-wm xcb-util-keysyms git less base-devel git curl zip unzip tar cmake ninja
```


### Step 2. Clone the repo
```sh
git clone --recurse-submodules https://github.com/Sebanisu/Field-Map-Editor.git Field-Map-Editor
```

### Step 3. Bootstrap vcpkg
```sh
cd Field-Map-Editor
./vcpkg/bootstrap-vcpkg.sh
```

### Step 4. Configure CMAKE
```sh
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
```

### Step 5. Build
```sh
cmake --build .
cd ..
```

## Additional Notes
- The compiled binaries can be found in the `./build/bin/` directory.
