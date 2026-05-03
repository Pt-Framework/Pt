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
- Use `@brief` for the one-line summary. Place it right after `/**`
- Use `@ingroup <group>` to assign the class/function to a module group.
- Use `@related <ClassName>` to associate operators and free functions with a class when appropriate.
- Do not use structural keywords like `@class` when the context is already unambiguously clear to Doxygen.
- Module groups are defined with `@defgroup` in the module's `Api.h`.

## Example

```cpp
/** @brief Represents a connection between a Signal and a Slot.
    @ingroup sigslot
*/
class PT_API Connection
{
    public:
        /** @brief Returns true if the connection is open.
        */
        bool isValid() const;

        /** @brief Closes the connection.
        */
        void close();
};
```

## Group Naming

| Module | Group examples |
|--------|---------------|
| Pt (core) | `sigslot`, `Allocator`, `BasicTypes`, `Serialization` |
| Pt::Gfx | `GfxTypes`, `GfxPaint` |

Groups are listed in each module's `Api.h` header.
