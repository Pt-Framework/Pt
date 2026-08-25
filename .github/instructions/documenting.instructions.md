---
applyTo: "**/*.h"
description: "API Documentation"
---

# Documentation Structure

Doxygen documentation for classes/functions/groups is the foundation.
Pages, the Website and agent instructions build on top of it. Pages assemble
the Doxygen content for human readers, and instructions files index it by
features for agents.

# API Documentation

- Prefer brief only documentation in simple cases.
- All API documentation (namespaces, classes, methods, enums, free
  functions) belongs in the public header files, not in `.cpp` files.
- Document every public namespace, class, funtions using Doxygen.
- Assign each class/interface/function to a Doxygen group using `@ingroup`.
- Internal helpers in `.cpp` files may use brief comments but do not need
  Doxygen markup.
- Class-specific documentation belongs in the class comment in its header.
- Cross-cutting concepts, patterns, and examples that span multiple
  classes belong in a doxygen group.
- Module-level concepts belong in the `@namespace` comment in the
  module's `Api.h`.
- Group IDs are derived from the C++ namespace by replacing `::` with `-`:
  `Ns::` -> `Ns-<Feature>`, `Ns::Sub::` -> `Ns-Sub-<Feature>`.
- Doxygen Group headers live in the module's public include directory, named
  `Api-<Feature>.h`, e.g. `include/<Project>/<Module>/Api-<Feature>.h`.
  Core module groups live in `include/<Project>/Api-<Feature>.h`.
- Each `Api-<Feature>.h` file contains exactly one `@defgroup` block,
  wrapped in include guards (`#ifndef PT[_MODULE]_API_FEATURE_H`).
- Class-doc overrides use `Api-<ClassName>.h` in the same include directory
  as the real header. These provide Doxygen documentation for classes that
  are typedefs or template specializations where Doxygen generates poor
  output from the real declaration.

# User Documentation Pages

- Doxygen Page files live in `doc/pages/` and compose the Doxygen API and group
  documentation into readable user documentation.
- Page file names are lowercase, e.g. `pt-signals.page`.
- Page IDs use a `-Page` suffix to distinguish them from group IDs.
  - Example: group `Pt-Signals` → page `Pt-Signals-Page`.
  - Section anchors use the page ID as prefix with hyphens:
    `Pt-Signals-Page-Signals`.
- Pages contain **no new content** — only `@copydetails` references.
- Use `@copydetails <GroupId>` in a page to pull in group documentation.
- Use `@section <anchor> <Title>` to introduce page subsections.
- Use `@copydetails <Qualified::Name>` in a page to pull in class or function
  documentation.

# Agent Instructions

- One `.instructions.md` file per **high-level feature set** (covering one
  or more related groups), e.g. `<Module>.instructions.md` covers all
  `<Module>-*` groups.
- These files index the Doxygen foundation for agents: they map features
  and tasks to the relevant headers and `Api-<Feature>.h` group files.
- They do not contain documentation or explanations — those belong in
  headers and group files.

# Website

- Doxygen generates HTML into `doc/website/htdocs/`. Do not edit those files
  by hand.
- The website root files in `doc/website/` (e.g. `index.html`, `docs.html`)
  contain hand-written navigation links to Doxygen output in `doc/website/*.html`.
  - Doxygen `\page <id>` produces `htdocs/<id>.html`.
  - Doxygen `@defgroup <id>` produces `htdocs/group__<id>.html`.

# Doxygen Style

- ALWAYS use `/** ... */` block comments for namespaces, classes, member
  functions and standalone functions.
- Place the closing `*/` of block comments on the next line.
- Do not use leading asterisks (*) on intermediate lines inside block comments.
- Do not document forward declarations.
- Use `@brief` for the one-line summary. Place it right after `/**`.
- Place the detailed description after a blank line following `@brief`,
  indented to align flush with the `@brief` command (4 spaces from `/**`).
- If Doxygen commands (e.g. `@ingroup`, `@param`, `@return`) follow the
  detailed description, separate them with a blank line.
- Use `@ingroup <group>` to assign the class/function to a module group.
- Use `@related <ClassName>` to associate operators and free functions with
  a class when appropriate.
- Escape class names, namespace-qualified names, and function names in
  prose with `%` unless an explicit Doxygen link is desired.
  - Examples: `%MyClass`, `%MyNamespace::MyModule::MyClass`, `%MyClass::begin()`
- Do not use structural keywords like `@class` when the context is already
  unambiguously clear to Doxygen.
- Feature groups are defined with `@defgroup`.
- Each `@defgroup` block contains:
  - `@brief` one-line summary
  - Detailed description of the feature area
  - Usage rules and design guidance that apply to the whole group
  - `@code` / `@endcode` example(s) showing the canonical usage pattern

# Header Example

```cpp

namespace Ns {

/** @brief Brief description of the class.

    Detailed description of the %MyClass class.

    @ingroup Ns-MyFeature
*/
class MyClass
{
  public:
    /** @brief Constructor.
    */
    MyClass();

    /** @brief No copy constructor.
    */
    MyClass(const MyClass&) = delete;

    /** @brief Destructor.
    */
    virtual ~MyClass();

    /** @brief Multiplies @a a and @a b and returns the result.
    */
    int multiply(int a, int b);

    /** @brief This method does something complicated.

        Detailed description of the method.

        @param name Description of parameter name.
        @param b Description of parameter b.

        @return Description of the result.

        @throw std::invalid_argument if @a  b is 0.
    */
    float somethingComplicated(const std::string& name, float b);
};

}
```

# Group Example

```cpp
/** @defgroup Ns-MyFeature Feature Name

    @brief Brief description of the feature group.

    Detailed description of the feature area, usage rules, and design guidance.
*/
```

# Page Example

```
/** \page Cosmo-Activation-Page Activation

    @copydetails Cosmo-Activation

    @section Cosmo-Activation-Page-IActivate IActivate
    @copydetails Cosmo::IActivate

    @section Cosmo-Activation-Page-IActivator IActivator
    @copydetails Cosmo::IActivator
*/
```
