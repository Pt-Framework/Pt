---
name: pt-building
description: Building the Pt project with jam. Use this when asked about configuring, building or cleaning the project.
---

# Building with Jam

- The Pt project uses a custom jam-based build
- Windows: `jam.bat`, Linux/macOS: `jam.sh`
- All platforms use the same basic jam arguments
- some platforms may have additional platform-specific options
- The examples below use `jam.bat`; substitute `jam.sh` accordingly

## Project Layout

- Public headers live in `include/Pt/<Module>/`
- `<Module>` matches the module namespace, e.g. `include/Pt/System/` for `Pt::System`
- Implementation files live in `src/<BaseName>/`
- `<BaseName>` is the library base name, e.g. `src/Pt-System/` for `Pt-System.dll` / `libPt-System.so`
- Each module has a `Jamfile` in `src/<BaseName>/`
- Test executables are built from `src/<BaseName>/tests/Jamfile`
- Examples: `src/Pt/` (core), `src/Pt-Gfx/` (graphics), `src/Pt-System/` (system)
- Build output (binaries, libs) goes to `build/<CONFIG>/`
- Object files go to `tmp/<CONFIG>/<BaseName>/`
- `<CONFIG>` is the value of `-sCONFIG` passed to `jam.bat configure`

## 1. Configure

- `configure` and `switch` are arguments to the jam command
- `jam.bat configure` always creates a new build configuration
- `jam.bat switch` changes to an existing, previously configured build configuration
- Configure debug: `jam.bat configure -sCONFIG=debug --debug`
- Configure release: `jam.bat configure -sCONFIG=release --debug --optimize`
- Switch to debug: `jam.bat switch debug`
- Switch to release: `jam.bat switch release`

### Common Configure Arguments

- `-sCONFIG=<name>` — configuration name, determines the output subdirectory under `build/<name>/` and `tmp/<name>/`
- `--debug` — enable debug symbols
- `--optimize` — enable compiler optimizations

## 2. Build

- Build all modules: `jam.bat -q -j4`
- `-j4` for parallel compilation
- `-q` to stop on first error
- jam uses file timestamps to determine which files need to be rebuild

## 3. Clean

- Remove all build artifacts: `jam.bat clean`

## Adding Files to the Build

### Adding to an Executable (Main rule)

- Used in `src/<BaseName>/tests/Jamfile` for test executables
- Used in `src/<BaseName>/Jamfile` for normal executables
- Add the `.cpp` file to the `Main` source list:

```jam
Main Pt-Gfx-test : Pt-Gfx-test.cpp
                   Argb32Test.cpp
                   NewFile.cpp
                   ;
```

### Adding to a Shared Library (SharedLibrary rule)

- Used in `src/<BaseName>/Jamfile` for module libraries
- Add the `.cpp` file to the `SharedLibrary` source list:

```jam
SharedLibrary Pt : Atomicity.cpp
                   Connection.cpp
                   NewFile.cpp
                   ;
```

### Adding to a Static Library (Library rule)

- Add the `.cpp` file to the `Library` source list:

```jam
Library MyLib : Source1.cpp
               Source2.cpp
               NewFile.cpp
               ;
```
