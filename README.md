# PicSFML

PicSFML is a command-line utility made for quickly creating and building **SFML** projects.

It automates the annoying setup process for SFML applications by generating project folders, configuration files, build directories, VSCode settings, linking SFML libraries, copying DLLs, and handling resources automatically.

Built for Windows and designed mainly for fast prototyping and learning projects.

---

## Features

* Create ready-to-build SFML projects
* Automatic build system using GCC/MinGW
* Supports:
  * SFML 2.6.2
  * SFML 3.0.0
* Debug and Release builds
* Automatic DLL copying
* Automatic `Resources/` folder copying
* Optional VSCode configuration generation
* Optional SFML Audio module
* Optional SFML Network module ( just the SFML part )
* JSON project configuration system
* Custom include paths, binaries, and libraries
* Includes a lightweight utility/core framework
* Automatic set of the application icon
* Application versioning and windows metadata
 
---

## Included Core Utilities

PicSFML includes a small reusable framework inside `Core/`:

* `Application.h`
* `Input.h`
* `ResourceManager.h`
* `MusicPlayer.h`
* `SoundPlayer.h`
* `SoundSettings.h`
* `Utils.h`

Separate versions are included for both SFML 2.6.2 and SFML 3.0.0.

---

## Requirements

* Windows
* GCC / MinGW
* SFML 2.6.2 or SFML 3.0.0
* C++17

---

# Usage

## Create a Project

```bash
picsfml -c <project_path>
```

Example:

```bash
picsfml -c . -n Sandbox -sv 3.0.0 -vs --audio
```

### Create Arguments

| Argument    | Description                       |
| ----------- | ----------------------------------|
| `-n`        | Project name                      |
| `-o`        | Output executable name            |
| `-m`        | Main source filename              |
| `-g`        | Path to MinGW directory           |
| `-s`        | Path to SFML directory            |
| `-sv`       | SFML version (`2.6.2` or `3.0.0`) |
| `-vs`       | Generate VSCode configuration     |
| `--audio`   | Enable SFML audio module          |
| `--network` | Enable SFML network module        |

---

## Build a Project

```bash
picsfml -b <project_path>
```

### Build Arguments

| Argument | Description   |
| -------- | ------------- |
| `-d`     | Debug build   |
| `-r`     | Release build |

Example:

```bash
picsfml -b . -d
```

---

# Project Configuration

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
    }
}
```

You can add:

* Additional include directories
* Custom libraries
* Extra DLLs/binaries
* Flags for different build types

---

# What PicSFML Automatically Handles

When building a project PicSFML:

* Compiles the project
* Links SFML libraries
* Creates build folders
* Copies required SFML DLLs
* Copies GCC runtime DLLs
* Copies custom binaries
* Copies the `Resources/` folder

---

# Supported SFML Versions

| Version    | Supported |
| ---------- | --------- |
| SFML 2.6.2 | Yes       |
| SFML 3.0.0 | Yes       |
