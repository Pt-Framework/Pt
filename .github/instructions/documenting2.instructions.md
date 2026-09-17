---
description: "API Documentation. Doxygen groups, class comments, pages, and narrative reference prose."
---

# Voice

Write for a reader who uses the public API, not for its implementer.
Use plain, precise language in complete sentences. Keep real identifiers
as names (`--with-prebuilt`, `PREBUILT_ROOT`). Use one term for one
concept. Use established API terms such as object, instance, value,
parent, owner, lifetime, default, local, inherited, and override only
when they describe a public contract. Avoid informal wording, marketing
language, stacked jargon, and internal implementation details.

## Group and Class Detailed Descriptions

These texts are the chapter. Write them as accessible textbook or
technical-reference prose, not as briefs. Length follows the model. One
idea per paragraph. A paragraph may use several sentences to develop
that idea and lead into the next. Prefer an extra paragraph over a
comma-separated inventory. Do not use lists or extra headings inside a
detailed description unless the reader must choose between alternatives.

Include the context needed to use the API correctly. Explain this
feature's object model, contracts, ownership, lifetime, errors, and
ordering. An API technique that is part of the contract belongs here
(C linkage of an export, a cast the platform requires, matching
allocators). Do not teach general programming, and do not compare other
frameworks.

Use `@code` for the examples the chapter needs. An ordered explanation
of the model is not a recipe. Do not write cookbook steps ("When you
want to ..., first ..., then ..."). Do not write capability inventories
or front-loaded catalogs of neighboring types.

## Brief Reference Text

This short style is for member functions, parameters, enumerators, and
similar declaration comments. It is not the style of group or class
detailed descriptions.

Method briefs, parameters, and enumerators stay short and scannable.
Name the result or effect first. Use "Returns ..." for queries, "Sets ..."
or "Changes ..." for mutators, and "Creates ..." or "Adds ..." for factory
and registration operations. Do not begin a brief with "This function" or
repeat the method name in prose.

If a sentence needs more than two commas or an "and" chain of APIs, split
it. Document behavior, ownership, lifetime, side effects, errors, and
ordering only when they are not evident from the declaration. Do not
narrate internal execution steps. Do not walk through other members the
brief already covers.

# Where Documentation Lives

Doxygen comments in public headers are the foundation. Pages assemble
them for human readers. Agent instruction files index that foundation.
The website links to the generated HTML.

- Document every public namespace, class, function, enum, and public
  member in the public header, not in a `.cpp` file. Internal helpers in
  `.cpp` files may use brief comments but do not need Doxygen markup.
- A one-line `/** @brief ... */` is enough for a member when the
  declaration and brief suffice. Add detail, `@param`, and `@return`
  only for non-obvious behavior, contracts, errors, or complex usage.
  Refer to parameters with `@a <name>`.
- Assign an API to a group with `@ingroup` when it has a meaningful role
  in that group's reader task. An API may belong to more than one group.
  Do not assign an API to a group solely to classify it; it may remain
  outside a feature group.
- Module-level concepts belong in the `@namespace` comment in the
  module's `Api.h`.
- Group IDs replace `::` with `-`: `Ns::` -> `Ns-<Feature>`,
  `Ns::Sub::` -> `Ns-Sub-<Feature>`.
- Group headers live in the module's public include directory as
  `Api-<Feature>.h`. Core module groups live in
  `include/<Project>/Api-<Feature>.h`. Each file contains one `@defgroup`
  or `@addtogroup` block, wrapped in include guards
  (`#ifndef <PROJECT>[_MODULE]_API_FEATURE_H`).
- A main module group may declare ordered child groups with `@defgroup`
  and `@ingroup` when their order in Doxygen's Modules tree is part of
  the documentation structure. Each child `Api-<Feature>.h` then uses
  `@addtogroup`. Use this exception only for ordered direct children of
  a main module group.
- For typedefs or template specializations that Doxygen documents poorly,
  put class documentation in `Api-<ClassName>.h` next to the real header.

# Groups, Classes, and Pages

Organize groups around reader tasks, concepts, and public mechanisms, not
inventories of headers or types. Shared concepts belong in the group.
Local concepts belong on the owning API.

Each module has one module page. That page sorts the whole module: the
main group, every subgroup, and the types that deepen those groups.

A group detailed description is the essential chapter: what the feature
is for, then the object model in reading order, then the examples the
reader needs. Introduce a type only when the next sentence needs it. Do
not catalog the group's APIs.

A subgroup declares its parent with `@ingroup <ParentGroup>`. The parent
explains the module-wide model. The subgroup documents one reader task
within that model. Place every feature of the module on the module page
as a subgroup or a main-group chapter. Do not give a feature its own
page because it is short, long, or could ship separately. Decide
subgroup versus remaining in the parent by coupling: a subgroup needs
its own reader task or vocabulary. Do not create or merge a subgroup
merely because its documentation is short.

Do not split one essay across classes. The group tells the model once.
Each class answers one question that group left open. Shared contracts
belong once, in the group or on the first type that owns the mechanism.
Later types assume them. If two classes would answer the same question,
move that sentence to the group.

A class detailed description continues the group chapter when the page
copies it. Open with a hinge sentence that states this type's role in
the already-told model, then develop the questions that type leaves
open, in the same textbook voice as the group. Do not reteach the
group or recap earlier types. End so the next type on the page is
expected. The class page must still read as a coherent excerpt.

Copy a type onto the module page with `@copydetails` when that is
possible: the type deepens the group model, and its detailed
description can open as the next chapter beat. Prefer that
continuation. Do not force it. If the class text would reteach the
group, need glue of more than one thought, or cannot stand as the next
beat, keep the class short and keep the content in the group. A brief
is enough when the group already holds the content or the type adds no
new question; do not copy that type onto the page.

Pages live in `doc/pages/`. File names are lowercase. A module page ID
uses a `-Page` suffix (`Ns-MyModule` -> `Ns-MyModule-Page`). Section
anchors use the page ID as prefix (`Ns-MyModule-Page-MyFeature`).

The module page contains no concept, contract, usage rule, or example
that belongs in a group or class comment. It assembles that
documentation with `@copydetails`, not `@copydoc`. The page keeps the
section structure. The group detailed description is the chapter. Each
copied class detailed description must open as the next beat. A short
glue sentence may bridge sections. If that glue needs more than one
thought, do not copy the type: keep the class short and keep the
content in the group.

Copy the main group first, then subgroups, in reader order. The page
defines that order. Open with a short table of contents naming the main
sections that follow. Put the central type of the object model or reader
task where the reading order needs it, even if that is not inheritance
or `main()` order.

A group keeps its `@ingroup` identity on the module page. Reference it
from elsewhere with the section anchor that holds it
(`@ref <Page>-<Section>`), not a group-only page ID.

Use `@section` for subgroups and direct main-group chapters. Use
`@subsection` for types below a subgroup. Name the chapter after the
reader-facing role (`Widgets`, not `Views and Widget Services`).

Write in this order: outline the group as the essential chapter and the
page as questions; copy a type only when its detailed description
continues that chapter; otherwise keep the class short and leave the
content in the group; read the assembled page and cut repeats.

Guide pages (`jam-*.page`, `installing.page`, `tutorial.page`, ...)
contain original prose and follow the same voice. They must not repeat
API reference that already lives in a group, or content that already
lives on another page; point to it with `@ref` instead. Use `@code` for
commands and `@verbatim` for directory trees and URLs.

# Agent Instructions

One `.instructions.md` file per high-level feature set, covering one or
more related groups. These files index the Doxygen foundation for agents:
they map features and tasks to the relevant headers and `Api-<Feature>.h`
group files. They do not contain documentation or explanations; those
belong in headers and group files.

# Website

Doxygen generates HTML into `doc/website/htdocs/`. Do not edit those
files by hand. The website root files in `doc/website/` (`index.html`,
`docs.html`) contain hand-written navigation links to that output.

- `\page <id>` produces `htdocs/<id>.html`.
- `@defgroup <id>` produces `htdocs/group__<id>.html`.

Maintain the matching module box in `doc/website/docs.html` when adding
or changing a documented module.

# Doxygen Markup

- Use `/** ... */` block comments. Place the closing `*/` on the next
  line. Do not use leading asterisks on intermediate lines.
- Do not document forward declarations.
- Place `@brief` on the first line. After a blank line, indent the
  detailed description to align with `@brief` (4 spaces from `/**`).
- Separate later Doxygen commands (`@ingroup`, `@param`, `@return`) from
  the detailed description with a blank line.
- Use `@related <ClassName>` for operators and free functions when
  appropriate.
- Escape class names, namespace-qualified names, and function names in
  prose with `%` unless an explicit Doxygen link is desired
  (`%MyClass`, `%MyClass::begin()`).
- Do not use `@class` when the context is already clear to Doxygen.

# Examples

Group and class detailed descriptions use the connected chapter prose.
Member comments use the short brief form.

Dense inventory, do not write:

    A widget supplies identity, geometry, visibility, scaling, event
    dispatch, and pointer capture. Construct widgets only after an
    Application exists.

Connected prose:

    Every visual Forms object is a Widget. Application is not. A widget
    lives in a parent chain that reaches a Screen; that chain is a
    runtime relationship, not ownership. Geometry is logical: position
    is in parent coordinates, size is local.

```cpp
/** @brief Runtime root of a Forms user interface.

    %Application provides the Forms runtime. It is not a %Widget.
    Construct it before any widget, show the visual hierarchy, then
    enter the inherited event loop.

    The application does not own the windows or controls that use its
    services. Widgets remove themselves from their visual parent when
    they are destroyed.

    @ingroup Ns-MyFeature
*/
class MyClass
{
  public:
    /** @brief Divides @a value by @a divisor.

        @throw %std::invalid_argument if @a divisor is 0.
    */
    float divide(float value, float divisor);
};
```

```cpp
/** @defgroup Ns-MyFeature Feature Name

    @brief Brief description of the feature group.

    A Forms application constructs its visual hierarchy, shows its
    windows, and runs the event loop.

    %Application is the runtime root. Attaching a window or control
    does not transfer ownership. The code that creates an object keeps
    it alive while the hierarchy uses it.

    @code
    Pt::Forms::Application application(argc, argv);
    Pt::Forms::Window window;
    window.show();
    application.run();
    @endcode
*/
```

```
/** \page Ns-MyModule-Page Module Name

    @copydetails Ns-MyModule

    This chapter covers:

    - @ref Ns-MyModule-Page-MyFeature

    @section Ns-MyModule-Page-MyFeature Feature Name
    @copydetails Ns-MyFeature

    @subsection Ns-MyModule-Page-MyClass MyClass
    @copydetails Ns::MyClass
*/
```
