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

- Each module has a public `Api.h` header file with the forward declarations
  of its classes.

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

## Export Macros

All classes with out-of-line methods must be exported with the module's
API macro (e.g. `PT_API` for the core module, `PT_SYSTEM_API` for Pt-System):

```cpp
class PT_API MyClass {
    // ...
};
```

The macro is defined in the module's `Api.h` header:
- `PT_API_EXPORT` defined → `PT_API` expands to `PT_EXPORT` (building the library)
- Otherwise → `PT_API` expands to `PT_IMPORT` (consuming the library)

Pure template classes and inline-only classes do not need the export macro.

## Template Implementation Pattern

Separate template implementations into `.tpp` files. Include the `.tpp`
at the end of the `.h` file, inside the namespace:

```cpp
// Signal.h
namespace Pt {

class SignalBase : public Connectable { /* ... */ };

#include <Pt/Signal.tpp>

} // namespace Pt
```

## Adding a New Public Class

1. **Header**: Create `include/Pt/<Module>/NewClass.h`
   - Include guard: `#ifndef Pt_<Module>_NewClass_h` / `#define ...`
   - Add `#include <Pt/<Module>/Api.h>` if using the export macro
   - Add Doxygen `@brief` and `@ingroup`

2. **Implementation**: Create `src/Pt-<Module>/NewClass.cpp`
   - Include the public header: `#include <Pt/<Module>/NewClass.h>`

3. **Build**: Add `NewClass.cpp` to the source list in `src/Pt-<Module>/Jamfile`

4. **Forward declaration**: Add to `include/Pt/<Module>/Api.h` if other
   modules need the forward declaration

5. **Template code** (if applicable): Create `include/Pt/<Module>/NewClass.tpp`,
   include it at the end of `NewClass.h` inside the namespace

6. **Tests**: Add a test class in `src/Pt-<Module>/tests/NewClassTest.cpp`,
   register in `src/Pt-<Module>/tests/Jamfile`
   (see `testing.instructions.md` for the test pattern)

For the Pt core module, omit the `<Module>` path segment (e.g. `include/Pt/NewClass.h`,
`src/Pt/NewClass.cpp`, export macro `PT_API`).

