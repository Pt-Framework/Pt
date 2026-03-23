---
applyTo: "**/*.{h,cpp,Jamfile}"
description: "Project directory structure and coding architecture for the Pt project"
---

# Coding Guidelines

## Language Features

- Use C++14 features where appropriate

## Project Namespaces

- The project namespace and the namespace of the core module is Pt
- Each Module has its own namespace e.g. Pt::System, Pt::Net, Pt::Xml, Pt::Gfx

## Directory Layout

- Public headers live in `include/Pt/`
    - `include/Pt/` for `Pt` core module

- Public module headers live in  `include/Pt/<Module>\`
  - `<Module>` matches the module namespace, e.g.
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

