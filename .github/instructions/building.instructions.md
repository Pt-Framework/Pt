---
applyTo: "**/*.{h,cpp,Jamfile}"
description: "Build system"
---

# Building with Jam

- The Pt project uses a custom jam-based build
- Windows: `jam.bat`, Linux/macOS: `jam.sh`
- All platforms use the same basic jam arguments
- Some platforms may have additional platform-specific options
- The examples below use `jam.bat`; substitute `jam.sh` accordingly

- NEVER use VS Code tasks (run_task) to build.
- ALWAYS check the exit code of the actual standalone build command to determine build success.
- ALWAYS check the exit code after every build command — no exceptions. 
- NEVER rely on build output alone.
- ALWAYS run the build command without any further processing.
- ALWAYS perform a full global build to verify that dependent modules are not broken. 
- NEVER only build the local module or test target.

## Project Layout

- Public headers live in `include/Pt/<Module>/`
- `<Module>` matches the module namespace, e.g.
  - `include/Pt/` for `Pt`
  - `include/Pt/System/` for `Pt::System`
  - `include/Pt/Net/` for `Pt::Net`

- Implementation files live in `src/<BaseName>/`
- Each module has its `Jamfile` in `src/<BaseName>/`
- `<BaseName>` is the library base name, e.g.
  - Core Module: `src/Pt/` for `Pt.dll` / `libPt.so`
  - System Module: `src/Pt-System/` for `Pt-System.dll` / `libPt-System.so`

- Test sources live in `src/<BaseName>/tests`
- Test have its `Jamfile` in `src/<BaseName>/tests/Jamfile`

- Build output (binaries, libs) goes to `build/<CONFIG>/`
- Object files go to `tmp/<CONFIG>/<BaseName>/`
- `<CONFIG>` is the value of `-sCONFIG` passed to `jam.bat configure`
- Test executables are located at `build/<CONFIG>/<test-executable-name>`.

## 1. Configuring the Build

- Configure debug build: `jam.bat configure -sCONFIG=debug --debug`
- Configure release vuild: `jam.bat configure -sCONFIG=release --debug --optimize`
- Switch to debug configuration: `jam.bat switch debug`
- Switch to release configuration: `jam.bat switch release`

- `jam.bat configure` always creates a new build configuration
- `jam.bat switch` changes to an existing, previously configured build configuration
- `configure` and `switch` are arguments to the jam command
- `-sCONFIG=<name>` sets configuration name, determines the output subdirectory
  under `build/<name>/` and `tmp/<name>/`
- Run `jam.bat switch <name>` before building to confirm the active configuration.
- `--debug` — enable debug symbols
- `--optimize` — enable compiler optimizations

## 2. Build

- Build all: `jam.bat -q -j4`

- `-j4` for parallel compilation
- `-q` to stop on first error

- jam uses file timestamps to determine which files need to be rebuild

## 3. Clean

- Remove all build artifacts: `jam.bat clean`

## Adding Files to the Build

### Adding to an Executable (Main rule)

- Used in `src/<BaseName>/tests/Jamfile` for test executables.
- Used in `src/<BaseName>/Jamfile` for normal executables.

- The executable's base name is first argument for the the `Main` rule.
- For windows the suffix `.exe` is added to the executable base name.

- Add the `.cpp` files to the `Main` source list after the `:`:

```Jamfile
Main Pt-Gfx-test : Pt-Gfx-test.cpp
                   Argb32Test.cpp
                   NewFile.cpp ;
```

- Spaces around `:` and before `;` are required by the Jamfile syntax.

### Adding to a Shared Library (SharedLibrary rule)

- Used in `src/<BaseName>/Jamfile` for module libraries
- Add the `.cpp` file to the `SharedLibrary` source list:

```Jamfile
SharedLibrary Pt : Atomicity.cpp
                   Connection.cpp
                   NewFile.cpp
                   ;
```

### Adding to a Static Library (Library rule)

- Add the `.cpp` file to the `Library` source list:

```Jamfile
Library MyLib : Source1.cpp
               Source2.cpp
               NewFile.cpp
               ;
```
