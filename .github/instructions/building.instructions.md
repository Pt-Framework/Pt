---
description: "Build system & setup"
---

# Building with Jam

- The Pt project uses a custom jam-based build.
- Windows: `jam.bat`, Linux/macOS: `jam.sh`.
- `jam.bat` is located in the top level directory.
- All platforms use the same basic jam arguments.
- Some platforms may have additional platform-specific options.
- The examples below use `jam.bat`; substitute `jam.sh` accordingly.
- NEVER assume that the terminal starts in the correct project root.
- ALWAYS change to the repository root that contains `jam.bat` before running.
  any configure, switch, build, clean, or verification command.
- NEVER use VS Code tasks (run_task) to build.
- ALWAYS check the exit code of the actual standalone build command to determine build success.
- ALWAYS check the exit code after every build command — no exceptions.
- NEVER rely on build output alone.
- ALWAYS run the build command without any further processing.
- ALWAYS perform a full global build to verify that dependent modules are not broken.
- NEVER only build the local module or test target.

# Configuring the Build

- Configure debug build:
  `jam.bat configure -sCONFIG=debug --debug`

- Configure release vuild:
  `jam.bat configure -sCONFIG=release --debug --optimize`

- Switch to debug configuration:
  `jam.bat switch debug`

- Switch to release configuration:
  `jam.bat switch release`

- `jam.bat configure` always creates a new build configuration
- `jam.bat switch` changes to an existing, previously configured build configuration
- `configure` and `switch` are arguments to the jam command
- `-sCONFIG=<Config>` sets configuration name, determines the output subdirectory
  under `build/<Config>/` and `tmp/<Config>/`
- Run `jam.bat switch <Config>` before building to confirm the active configuration.
- `--debug` — enable debug symbols
- `--optimize` — enable compiler optimizations

## Pt-Forms UI Selection

- `PT_FORMS_IMPL` is a derived configure output and must not be supplied as user input.
- Use `-sPT_FORMS_PLATFORM=<platform>` only when a variable-based platform override is needed.
- Supported platform flags:
  - `--with-x11`
  - `--with-linux-fb`
  - `--with-linux-drm`
- Supported renderer flags:
  - `--with-direct2d`
  - `--with-gdi`
  - `--with-vulkan`
- Native defaults:
  - Windows: `win32` + `direct2d`
  - Linux: `linux-fb`
  - macOS: `cocoa`
  - Emscripten: `emscripten`
- Removed flags that must no longer be used:
  - `--with-win32-direct2d`
  - `--with-win32-gdi`
  - `--with-win32-raster`

# Building the Artifacts

- To build the currently selected configuration:
  - `jam.bat -q -j4`
  - `-j4` for parallel compilation (4 parallel tasks)
  - `-q` to stop on first error

- use higher numbers for `-j` to increase build speed or omit `-j` when analyzing build errors
- jam uses file timestamps to determine which files need to be rebuild

# Cleaning the Build

- Remove all build artifacts:
  `jam.bat clean`

# Build Setup

- Jamfiles live next to the source files in their module directory (`src/<Module>/Jamfile`).
- Spaces around `:` and before `;` are required by the Jamfile syntax.
- Comments in Jamfiles use `#`.

The following Jam variables control where build artifacts are placed:
  - Use `MakeLocate <target> : <dir>` in Jamfiles to set the output directory,
  - and `MakeInstall <target> : <dir>` in Jamfiles to set the install destination.

| Use for                      | Build path                        | Variable                  |
|------------------------------|-----------------------------------|---------------------------|
| Object files                 | `tmp/<Config>/`                   | `PT_OUTPUT_OBJECT`        |
| Executables                  | `build/<Config>/`                 | `PT_OUTPUT_TARGET`        |
| Shared libraries             | `build/<Config>/`                 | `PT_OUTPUT_TARGET`        |
| Install dest for executables | `deploy/<Config>/`                | `PT_INSTALL_BINDIR`       |
| Install dest for libraries   | `deploy/<Config>/`                | `PT_INSTALL_LIBDIR`       |

# Sub-Directory Specific Setup

Every module Jamfile starts with:

```Jamfile
SubDir PT_TOP src <Module-Name> ;

SubDirHdrs $(PT_TOP)/include ;
SubDirHdrs $(<DEP>_INCLUDES) ; # repeat for each dependency
SubDirObjects $(PT_OUTPUT_OBJECT) <Module-Name> ;
```

- `SubDir` declares the current directory relative to the project root.
- `SubDirHdrs` adds include search paths
- `SubDirObjects` directs object files to `$(PT_OUTPUT_OBJECT)/<Module-Name>/`.

For shared libraries that export a public API, also add:

```Jamfile
SubDirC++Flags [ FDefines <MODULE_NAME>_API_EXPORT ] ;
```

# Executables (Main rule)

Add the `.cpp` files to the `Main` source list after the `:`:

```Jamfile
Main <executable-name> : Source.cpp
                         NewFile.cpp ;
```

- The executable's base name is the first argument for the `Main` rule.
- On Windows, the binary produced by the build automatically gets the `.exe` suffix.

# Shared Libraries (SharedLibrary rule)

Add the `.cpp` files to the `SharedLibrary` source list after the `:`:

```Jamfile
SharedLibrary <library-name> : Source.cpp
                               NewFile.cpp
                               ;
```

- The library's base name is the first argument for the `SharedLibrary` rule.

# Static Libraries (Library rule)

Add the `.cpp` files to the `Library` source list after the `:`:

```Jamfile
Library <library-name> : Source1.cpp
                         Source2.cpp
                         NewFile.cpp
                         ;
```

- The library's base name is the first argument for the `Library` rule.

# Linking Against Internal Libraries (LinkSharedLibraries)

Use `LinkSharedLibraries` to link libraries built within this project:

```Jamfile
LinkSharedLibraries <target-name> : <Dep1> <Dep2> ;
```

- `<target-name>` is the name of a `Main` or `SharedLibrary` target defined in the build.
- `<Dep1>`, `<Dep2>` are `SharedLibrary` target names from this project.
- NEVER use `LinkSharedLibraries` for external/third-party libraries.

# Linking Against External Libraries (LinkLibs / LinkFlags)

Use `LinkLibs` and `LinkFlags` for pre-built external libraries and frameworks:

```Jamfile
LinkLibs <target-name> : $(<DEP>_LINKLIBS) ;
LinkFlags <target-name> : $(<DEP>_LINKFLAGS) ;
```

- The build system provides ready-made variables for each supported external dependency.
- Look up the available variables in the existing Jamfiles or in `Jamconfigure`.
- Only include variables for dependencies the module actually uses.

# Registering a Jamfile

A new module's Jamfile is not discovered automatically. Add a `SubInclude` line
next to the other `SubInclude` lines in the top-level `Jamfile`:

```Jamfile
SubInclude PT_TOP src <Module-Name> ;
```

Sub-directories (e.g. `tests/`) within a module are registered the same way,
but the `SubInclude` goes into the module's own Jamfile, not the top-level one:

```Jamfile
SubInclude PT_TOP src <Module-Name> tests ;
```
