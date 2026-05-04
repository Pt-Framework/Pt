---
applyTo: "**/*.h"
description: "Doxygen documentation conventions for public headers."
---

# Documenting

## Where to Document

- All public API documentation belongs in the **public header files**
  (`include/Pt/` or `include/Pt/<Module>/`), not in `.cpp` files.
- Internal helpers in `.cpp` files may use brief comments but do not
  need Doxygen markup.

## Doxygen Style

- ALWAYS Use `/** ... */` block comments for namespaces, classes, member functions and standalone functions.
- Place the closing `*/` of block comments on the next line.
- Do not use leading asterisks (*) on intermediate lines inside block comments.
- Do not document forward declarations.
- Use `@brief` for the one-line summary. Place it right after `/**`.
- Place the detailed description after a blank line following `@brief`,
  indented to align flush with the `@brief` command (4 spaces from `/**`).
- If Doxygen commands (e.g. `@ingroup`, `@param`, `@return`) follow the detailed description, separate them with a blank line.
- Use `@ingroup <group>` to assign the class/function to a module group.
- Use `@related <ClassName>` to associate operators and free functions with a class when appropriate.
- Escape class names, namespace-qualified names, and function names in prose with `%` unless an explicit Doxygen link is desired.
  - Examples: `%Signal`, `%Pt::Gfx::Painter`, `%Painter::begin()`
- Do not use structural keywords like `@class` when the context is already unambiguously clear to Doxygen.
- Module groups are defined with `@defgroup` in `doc/groups/*.dox`.

## Example

```cpp
/** @brief Represents a connection between a %Signal and a %Slot.

    A %Connection object is returned by %Signal::connect() and can be
    used to close the connection at a later time.

    @ingroup sigslot
*/
class PT_API Connection
{
    public:
        /** @brief Returns true if the connection is open.
        */
        bool isValid() const;

        /** @brief Closes the connection.

            After calling this method the connection is no longer active
            and the associated slot will not receive any further signals.
        */
        void close();
};
```

## Group Naming

Group IDs follow a namespace-derived prefix convention using hyphens:

- All groups use the `Pt-<Module>-` prefix scheme.
- `Pt` core module uses `Pt-` prefix — e.g. `Pt-SigSlot`, `Pt-Allocator`, `Pt-BasicTypes`
- Sub-modules use `Pt-<Module>-` prefix — e.g. `Pt-Gfx-Types`, `Pt-Gfx-Paint`, `Pt-System-Logging`

| Module | Group examples |
|--------|---------------|
| Pt (core) | `Pt-SigSlot`, `Pt-Allocator`, `Pt-BasicTypes`, `Pt-Serialization` |
| Pt::Gfx | `Pt-Gfx-Types`, `Pt-Gfx-Paint` |
| Pt::System | `Pt-System-Logging`, `Pt-System-FileSystem` |
