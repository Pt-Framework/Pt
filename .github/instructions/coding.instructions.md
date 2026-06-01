---
applyTo: "**/*.cpp,**/*.h"
description: "Coding Styles and Guidelines"
---

# Directory Layout

- Namespaces & Hierarchy:
  - Directory hierarchy must mirror namespace hierarchy
  - Root namespace: `<Project>` (Note: The project itself can act as a module)
  - Sub-namespaces: `<Project>::<Module>`
  - The project itself can act as a module

- Header & Public API:
  - Core module headers: `include/<Project>/`
  - Sub-module public headers: `include/<Project>/<Module>/`

- Implementation Files:
  - Core module sources: `src/<Project>/`
  - Sub-module sources: `src/<Project>-<Module>`

- Private API headers:
  - live next to source files
  - Core module: `src/<Project>/`
  - Sub-module: `src/<Project>-<Module>`

- Template Implementations:
  - in `NewClass.hpp` alongside `NewClass.h`, included at the end inside the namespace

- Tests:
  - Core module: `src/<Project>/tests`
  - Sub-module: `src/<Project>-<Module>`/tests

-  Build Output:
  - Build output: `build/<Config>/`
  - Object file: `tmp/<Config>/`

# Coding Style

- When modifying existing code, match surrounding style
- All comments, identifiers, log messages and exception messages in English
- Comments short and precise

- Naming Conventions:
  - Classes/Structs: `PascalCase`
  - Only Cosmo Component Interfaces: `I` prefix (e.g., `ISystem`)
  - Functions/Methods: `camelCase`
  - Member variables: `_` prefix (e.g., `_member`)
  - Virtual Methods/Callbacks: `on` prefix (e.g., `onActivate`)

- Braces & Control Flow:
  - Style: Allman-style (always new line for all braces, including `else`).
  - Else: Must always be on a new line (not attached to the closing brace).
  - Lambdas: Assign to a local variable before passing to an algorithm.

- Indentation & Layout:
  - Base: 4 spaces, no tabs.
  - Namespaces: No brace wrap, no indentation (content stays at column 0).
  - Class Layout: Indent `public`/`private` by 4 spaces from the `class` brace.
  - Initializers: Break constructor initializers before commas; do not pack them.
  - Two Blank Lines between member function definitions in .cpp files.
  - Two Blank Linesb before undocumented class/struct declarations in headers.

- Types & Symbols:
  - Pointers/References: Left-aligned (e.g., `Type* ptr`).
  - Unused Parameters: Comment out names: `void f(int /*unused*/) {}`.
  - Header Guards: Use `#ifndef PROJECT_MODULE_FILENAME_H` format.

- Spacing:
  - General: No space before parentheses (e.g., `if(cond)`, `func()`).
  - Logical Not: Always add a space after `!` (e.g., `! isValid`).
  - Inner Spacing: Add spaces inside parentheses when expressions are complex:
    - `if( x.isValid() )`
    - `::system( cmd.c_str() );`
    - `if( ! ptr )`
  - No inner spaces for trivial expressions:
    - `for(std::size_t i = 0; i < n; ++i)`
    - `if(byte < 256)`

- Formatting:
  - Line Length: Limit to 100 columns.
  - Templates: Always break declarations into multiple lines.
 
# Coding Guidelines

- Copyright Header:
  - Start every file with the copyright header from `include/<Project>/Api.h`.
  - Fill in the current year, leaving the author list intact.

- C++ Standard: Use C++14 features where appropriate.

- API Macros: Use the module's API macro (uppercase) from `Api.h` for non-inline symbols.
  - Core: `<PROJECT>_API` from `<Project>/Api.h`.
  - Sub-module: `<PROJECT>_<MODULE>_API` from `<Project>/<Module>/Api.h`.
  - Private classes: no export macro.
  ```cpp
  class MODULE_API MyClass { /*...*/ };
  MODULE_API void myFunction();
  ```

- Include Guards:
  - Derived from `<Project>`, `<Module>` and filename in uppercase:
    - Core: `<PROJECT>_<FILENAME>_H` (e.g. `MYPROJECT_MYCLASS_H`).
    - Sub-module: `<PROJECT>_<MODULE>_<FILENAME>_H` (e.g. `MYPROJECT_MYMODULE_MYCLASS_H`).

- Includes:
  - Keep original order (SortIncludes: Never).
  - Public headers (in `include/`): angle brackets `<...>`.
  - Private/local headers (in `src/`): quotes `"..."`.

- Local Variables for Long Expressions:
  - Extract repeated or deeply chained expressions into a local variable
  - Especially accessor chains like `Application::instance().style()` or
    `Application::instance().styleOptions()` should be stored in a local
    reference (e.g., `const Style& style = Application::instance().style();`).
  - Improves readability and avoids repeated evaluation of the same chain.

- Visual Studio Projects: 
  - Register new files also in the `.vcxproj` and `.vcxproj.filters` if they exist.
  - Filter structure in `.vcxproj.filters` mirrors the directory structure.