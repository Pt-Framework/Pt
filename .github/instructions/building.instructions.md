---
description: "Build system"
---

# Building with Jam

- The Pt project uses a custom jam-based build.
- Windows: `jam.bat`, Linux/macOS: `jam.sh`.
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

## Build Commands

- `jam.bat` is located in the top level directory.

### Configuring the Build

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

### Building the Artifacts

- To build the currently selected configuration:
  - `jam.bat -q -j4`
  - `-j4` for parallel compilation (4 parallel tasks)
  - `-q` to stop on first error

- use higher numbers for `-j` to increase build speed or omit `-j` when analyzing build errors
- jam uses file timestamps to determine which files need to be rebuild

### Cleaning the Build

- Remove all build artifacts: 
  `jam.bat clean`





## Build Setup

### Adding to an Executable (Main rule)

- Used in `src/<BaseName>/tests/Jamfile` for test executables.
- Used in `src/<BaseName>/Jamfile` for normal executables.

- The executable's base name is first argument for the the `Main` rule.
- On Windows, the binary produced by the build automatically gets the `.exe` suffix.

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
