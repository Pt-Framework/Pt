---
applyTo: "**/*.h"
description: "API Documentation"
---

# User Documentation

- Write for a reader who will use the feature, not for the person who
  implemented it.
- Full sentences and short paragraphs. Lead with what the thing is or what
  the reader does, then the command, identifier, or example.
- Use plain language in running text. Keep real identifiers as names
  (`--with-prebuilt`, `PREBUILT_ROOT`). Do not stack jargon
  (not "relocatable ABI tree under the identity directory").

# Documentation Structure

Doxygen documentation for classes/functions/groups is the foundation.
Pages, the Website and agent instructions build on top of it. Pages assemble
the Doxygen content for human readers, and instructions files index it by
features for agents.

Organize groups and pages around reader tasks, concepts, public mechanisms,
and design principles. They are not mechanical inventories of headers or
types. Explain a concept in a group when it spans multiple APIs, or in the
detailed documentation of its owning API when it is local to that API.

A group overview may summarize the whole feature area. Keep it to concepts,
mechanisms, principles, and usage.

A page assembles a group's overview and its class sections into one
document. Section order and titles are the reading order. When a page
assembles more than one group, open the page with a short table of
contents naming the sections that follow, instead of relying on
cross-references buried in each group.

Copied class documentation is a chapter of that document, not an appendix
of type summaries. It must deepen the group overview and still stand
alone on the class reference page.

# API Documentation

- Prefer compact documentation. Use a one-line `/** @brief ... */` comment
  when the signature and brief text are sufficient. Add detailed descriptions,
  `@param`, and `@return` only for non-obvious behavior, contracts, errors,
  or complex usage, not for trivial members (default constructors, destructors,
  simple getters/setters). The text must still cover relevant arguments, return
  value, and exceptions. Refer to parameters with `@a <name>`.
- All API documentation (namespaces, classes, methods, enums, free
  functions) belongs in the public header files, not in `.cpp` files.
- Document every public namespace, class, function, enum, and public member
  using Doxygen.
- Assign an API to a Doxygen group with `@ingroup` when it has a meaningful
  role in that group's reader task, concept, or public mechanism. An API may
  belong to more than one group. Do not assign an API to a group solely to
  classify it; it may remain outside a feature group.
- Internal helpers in `.cpp` files may use brief comments but do not need
  Doxygen markup.
- Give a class a detailed description when its role, ownership, lifecycle,
  extension point, or interaction with another type is not self-evident,
  or when a page copies that class as a chapter. Lead with the type's role,
  then the contract the group overview does not already state: ownership,
  lifecycle, usage, and whether the reader derives from it.
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
- API topic pages contain no concept, contract, usage rule, or example
  that belongs in a group or class comment. They assemble that
  documentation with structural Doxygen markup and `@copydetails`.
- Use `@copydetails`, not `@copydoc`, to pull in group, class, or function
  documentation. The page keeps the section structure; the brief is not copied,
  so the detailed description must open as the chapter.
- A page may copy multiple groups and individual namespaces, classes, enums,
  or functions. A Doxygen group may appear on multiple pages. There is no
  required one-to-one relationship between a page and a group.
- When assembling pages with @copydetails, ensure sections flow logically
  like a technical paper or article. Transitional prose and short glue may
  bridge sections, but the content itself always stays in the copied group
  or class doxygen comment.
- Decide whether a group gets its own page or folds into another page's
  section by coupling, not by counting files elsewhere in the repository.
  Fold a group in only when its subject is inseparable from the target
  page's object model, for example a base class the page already
  documents (`Widget` deriving from `Responder`); do not fold it in
  merely because the group is short.
- Test coupling with two questions: could the mechanism plausibly ship as
  a separate, swappable library or extension (own page), and does it need
  a vocabulary beyond the target page's base types (own page)? A short
  group is a signal to re-check its placement, never a reason by itself
  to merge it.
- A group keeps its `@ingroup` identity no matter which page copies it.
  Reference it from elsewhere with the section anchor that actually holds
  it (`@ref <Page>-<Section>`), not a group-only page ID, once it has no
  page of its own.
- Give each copied type its own chapter on the appropriate heading level:
  use `@section` on a single-group page and `@subsection` below a copied
  group section on a multi-group page. Name the chapter after the
  reader-facing role, not after an inventory label (`Widgets`, not `Views
  and Widget Services`).
- Put the central type of the object model or reader task where the
  reading order needs it, even if that is not inheritance or `main()`
  order.
- Guide pages (`jam-*.page`, `installing.page`, `tutorial.page`, ...)
  contain original prose and follow User Documentation. They must not
  repeat API reference that already lives in a group, or content that
  already lives on another page; point to it with `@ref` instead.
- `@code` for commands, `@verbatim` for directory trees and URLs.

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
- Use `@related <ClassName>` to associate operators and free functions with
  a class when appropriate.
- Escape class names, namespace-qualified names, and function names in
  prose with `%` unless an explicit Doxygen link is desired.
  - Examples: `%MyClass`, `%MyNamespace::MyModule::MyClass`, `%MyClass::begin()`
- Do not use structural keywords like `@class` when the context is already
  unambiguously clear to Doxygen.
- Each `@defgroup` block contains:
  - `@brief` one-line summary
  - A complete overview of the reader task, concepts, public mechanisms,
    principles, usage rules, and design guidance (see Documentation
    Structure)
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

  Complete overview of the feature area, its public mechanisms, usage
  rules, and design guidance.
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
