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

- Use `/** ... */` block comments for classes and standalone functions.
- Use `@brief` for the one-line summary.
- Use `@ingroup <group>` to assign the class/function to a module group.
- Use `//!` for short inline member documentation.
- Module groups are defined with `@defgroup` in the module's `Api.h`.

## Example

```cpp
/** @brief Represents a connection between a Signal and a Slot.
    @ingroup sigslot
*/
class PT_API Connection
{
    public:
        //! Returns true if the connection is open.
        bool isValid() const;

        //! Closes the connection.
        void close();
};
```

## Group Naming

| Module | Group examples |
|--------|---------------|
| Pt (core) | `sigslot`, `Allocator`, `BasicTypes`, `Serialization` |
| Pt::Gfx | `GfxTypes`, `GfxPaint` |

Groups are listed in each module's `Api.h` header.
