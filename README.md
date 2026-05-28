# PicSFML

PicSFML is a command-line utility for quickly creating and building **SFML** projects on Windows.

It automates the repetitive setup process for SFML applications by generating project structures, handling compiler configuration, linking SFML libraries, copying required DLLs, generating VSCode settings, managing resources, and maintaining a project configuration system.

Designed primarily for fast prototyping, learning projects, and lightweight game/application development.

---

# Features

* Create ready-to-build SFML projects
* Automatic GCC/MinGW build system
* Supports:

  * SFML 2.6.2
  * SFML 3.0.0
* Debug and Release builds
* Automatic SFML DLL copying
* Automatic GCC runtime DLL copying
* Automatic `Resources/` folder copying
* Optional VSCode configuration generation
* Optional SFML Audio module
* Optional SFML Network module
* JSON-based project configuration
* Custom include paths, libraries, and binaries
* Lightweight reusable utility framework
* Automatic application icon setup
* Application version metadata support
* Console configuration editing
* Project congiguration list in console

---

# Included Core Utilities

PicSFML includes a lightweight reusable framework inside the `Core/` directory.

Included utilities:

* `Application.h`
* `Input.h`
* `MusicPlayer.h`
* `ResourceManager.h`
* `SoundPlayer.h`
* `SoundSettings.h`
* `Utils.h`

Separate versions are included for both SFML 2.6.2 and SFML 3.0.0.

---

# Requirements

* Windows
* GCC / MinGW
* SFML 2.6.2 or SFML 3.0.0
* C++17

---

# Command Overview

```bash
picsfml [option] <path> [arguments] (values) ...
```

| Parameter | Description                                               |
| --------- | --------------------------------------------------------- |
| `<path>`  | Path to the project directory (`.` for current directory) |

---

# Create a Project

```bash
picsfml -c <path>
```

or

```bash
picsfml --create <path>
```

## Create Arguments

| Argument                         | Description                       |
| -------------------------------- | --------------------------------- |
| `-n`, `--name <string>`          | Project name                      |
| `-o`, `--output <string>`        | Output executable name            |
| `-m`, `--main <string>`          | Main source filename              |
| `-g`, `--gcc <string>`           | Path to MinGW directory           |
| `-s`, `--sfml <string>`          | Path to SFML directory            |
| `-sv`, `--sfml_version <string>` | SFML version (`2.6.2` or `3.0.0`) |
| `--vscode`                       | Generate VSCode configuration     |
| `--audio`                        | Enable SFML audio module          |
| `--network`                      | Enable SFML network module        |

## Example

```bash
picsfml -c . -n Sandbox -sv 3.0.0 --vscode --audio
```

---

# Build a Project

```bash
picsfml -b <path>
```

or

```bash
picsfml --build <path>
```

## Build Arguments

| Argument          | Description   |
| ----------------- | ------------- |
| `-d`, `--debug`   | Debug build   |
| `-r`, `--release` | Release build |

## Examples

### Debug Build

```bash
picsfml -b . -d
```

### Release Build

```bash
picsfml -b . -r
```

---

# List Project Configuration

```bash
picsfml -l <path>
```

or

```bash
picsfml --list <path>
```

## Create Arguments

| Argument                         | Description                       |
| ---------------------------------| --------------------------------- |
| `--all`                          | Show all the information          |
| `-n`, `--name`                   | Show project name                 |
| `-o`, `--output`                 | Show project output               |
| `-m`, `--main`                   | Show main file                    |
| `-g`, `--gcc`                    | Show GCC path                     |
| `-s`, `--sfml`                   | Show SFML path                    |
| `-sv`, `--sfml_version`          | Show SFML version                 |
| `-av`, `--application_version`   | Show application version          |
| `-l`, `--library`                | Show project libraries            |
| `-i`, `--include`                | Show project include              |
| `-b`, `--binary`                 | Show project binaries             |
| `-df`, `--debug_flags`           | Show debug build flags            |
| `-rf`, `--release_flags`         | Show release build flags          |
| `--vscode`                       | Show using VSCode check           |
| `--audio`                        | Show using audio check            |
| `--network`                      | Show using network check          |

## Example

```bash
picsfml -l . -all
```

---

# Modify Project Configuration

Modify values inside the `.picsfml_config` file directly from the command line.

```bash
picsfml -s <path>
```

or

```bash
picsfml --set <path>
```

---

# Configuration Commands

## Project Properties

### Project Name

```bash
picsfml -s . -n --set Sandbox
```

```bash
picsfml -s . -n --clear
```

### Output Executable Name

```bash
picsfml -s . -o --set Game
```

### Main Source File

```bash
picsfml -s . -m --set main.cpp
```

### GCC Path

```bash
picsfml -s . -g --set "C:/mingw64"
```

### SFML Path

```bash
picsfml -s . -s --set "C:/SFML-3.0.0"
```

### SFML Version

```bash
picsfml -s . -sv --set 3.0.0
```

### Application Version

```bash
picsfml -s . -av --set 1.0.0.0
```

---

# Include Directories

## Add Include Directory

```bash
picsfml -s . -i --add "Libraries/include"
```

## Remove Include Directory

```bash
picsfml -s . -i --remove "Libraries/include"
```

## Remove First Include

```bash
picsfml -s . -i --remove --front
```

## Remove Last Include

```bash
picsfml -s . -i --remove --back
```

---

# Libraries

## Add Library

```bash
picsfml -s . -l --add "-lopengl32"
```

## Remove Library

```bash
picsfml -s . -l --remove "-lopengl32"
```

---

# Binaries / DLLs

## Add Binary

```bash
picsfml -s . -b --add "example.dll"
```

## Remove Binary

```bash
picsfml -s . -b --remove "example.dll"
```

---

# Build Flags

## Debug Flags

```bash
picsfml -s . -df --set "-Wall"
```

## Release Flags

```bash
picsfml -s . -rf --set "-O2"
```

---

# Optional Features

## VSCode Support

Enable:

```bash
picsfml -s . --vscode --true
```

Disable:

```bash
picsfml -s . --vscode --false
```

## Audio Module

Enable:

```bash
picsfml -s . --audio --true
```

Disable:

```bash
picsfml -s . --audio --false
```

## Network Module

Enable:

```bash
picsfml -s . --network --true
```

Disable:

```bash
picsfml -s . --network --false
```

---

# Project Configuration File

Every generated project contains a `.picsfml_config` file.

Example:

```json
{
    "binary": [],
    "build": {
        "debug": {
            "flags": ""
        },
        "release": {
            "flags": ""
        }
    },
    "include": [],
    "library": [],
    "project": {
        "application_version": "1.0.0.0",
        "gcc": "",
        "main": "",
        "name": "",
        "output": "",
        "sfml": "",
        "sfml_version": "3.0.0",
        "use_audio": false,
        "use_network": false
    },
    "vscode": false,
    "version": "1.1.2"
}
```

The configuration file allows:

* Additional include directories
* Additional libraries
* Extra binaries / DLLs
* Custom compiler flags
* SFML module configuration
* Build customization

---

# What PicSFML Automatically Handles

During builds, PicSFML automatically:

* Compiles the project
* Links SFML libraries
* Creates build directories
* Copies required SFML DLLs
* Copies GCC runtime DLLs
* Copies custom binaries
* Copies the `Resources/` directory
* Creates the windwos resources
* Automatcaly creates the icon for windows

---

# Supported SFML Versions

| Version    | Supported |
| ---------- | --------- |
| SFML 2.6.2 | Yes       |
| SFML 3.0.0 | Yes       |

---

# Help & Version

Show help:

```bash
picsfml --help
```

Show version:

```bash
picsfml --version
```
