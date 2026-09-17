---
applyTo: "**/*.{h,md,page}"
description: "API Documentation"
---

# Voice

Write for a reader who uses the public API, not for its implementer.
Use plain, precise language in complete sentences. Keep real identifiers
as names (`--with-prebuilt`, `PREBUILT_ROOT`). Use one term for one
concept. Use established API terms such as object, instance, value,
parent, owner, lifetime, default, local, inherited, and override only
when they describe a public contract. Avoid informal wording, marketing
language, stacked jargon, and internal implementation details. Do not
teach general programming, and do not compare other frameworks.

# Chapter Versus Brief

Group and class detailed descriptions are the chapter. Write them as
accessible textbook or technical-reference prose, not as a brief and
not as a longer brief. Member functions, parameters, enumerators, and
similar declaration comments use the short brief form.

## Group and Class Detailed Descriptions

Develop the object model across several paragraphs. One contract per
paragraph. Extra sentences and subordinate clauses only develop that
contract: what it means, what it is not, what the caller must do. Do not
introduce a second type, relation, or rule in the same paragraph. A
paragraph that names two public types, two relations, or two rules is
too tight: split it.

A paragraph may use several sentences to develop that one contract and
lead into the next. Prefer three developed paragraphs over one paragraph
of three new facts. Do not write telegraphic sentences that only name a
fact. Subordinate clauses are wanted when they clarify the contract. Do
not use lists or extra headings inside a detailed description unless the
reader must choose between alternatives.

A restatement of members in order is still a brief. Split it until each
paragraph carries one contract the reader needs: role, ownership,
lifetime, errors, or ordering. Explain the object model, contracts,
ownership, lifetime, errors, and ordering. Include the context needed
to use the API correctly, including an API technique that is part of
the contract (C linkage of an export, a cast the platform requires,
matching allocators). A reader should understand the type from this
chapter without scanning every member.

Organize groups around reader tasks, concepts, and public mechanisms,
not inventories of headers or types. The group detailed description is
the essential chapter: what the feature is for, then the object model
in reading order. Introduce a type only when the next sentence needs
it. Do not catalog the group's APIs, write capability inventories, or
front-load neighboring types. An ordered explanation of the model is
not a recipe. Do not write cookbook steps ("When you want to ...,
first ..., then ...").

Do not split one essay across classes. The group tells the model once.
Shared concepts and shared contracts belong once, in the group or on
the first type that owns the mechanism. Local concepts belong on the
owning API. Later types assume them. If two classes would answer the
same question, move that sentence to the group.

Each class answers one question that the group left open. When a page
copies the class, open with a hinge sentence that states this type's
role in the already-told model, then develop those remaining questions
in several paragraphs, in the same textbook voice. Do not answer them
in one packed paragraph of members. Do not reteach the group or recap
earlier types. End so the next type on the page is expected. The class
page must still read as a coherent excerpt.

Keep a type to a brief when the group already holds the content or the
type adds no new question.

Use `@code` where the example belongs. A group example shows the group
model, not one type. A type-specific example belongs on the class that
owns the remaining question. A group needs no `@code` when prose is
enough or the examples live on the classes. Do not close the group with
a type-specific example.

## Brief Reference Text

Method briefs, parameters, and enumerators stay short and scannable.
Name the result or effect first. Use "Returns ..." for queries,
"Sets ..." or "Changes ..." for mutators, and "Creates ..." or
"Adds ..." for factory and registration operations. Do not begin a
brief with "This function" or repeat the method name in prose.

If a sentence needs more than two commas or an "and" chain of APIs,
split it. Document behavior, ownership, lifetime, side effects, errors,
and ordering only when they are not evident from the declaration. Do
not narrate internal execution steps. Do not walk through other members
the brief already covers.

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
- Pages live in `doc/pages/`. File names are lowercase. A module page ID
  uses a `-Page` suffix (`Ns-MyModule` -> `Ns-MyModule-Page`). Section
  anchors use the page ID as prefix (`Ns-MyModule-Page-MyFeature`).
- One `.instructions.md` file per high-level feature set, covering one or
  more related groups. These files map features and tasks to the relevant
  headers and `Api-<Feature>.h` group files. They do not contain
  documentation or explanations; those belong in headers and group files.
- Doxygen generates HTML into `doc/website/htdocs/`. Do not edit those
  files by hand. The website root files in `doc/website/` (`index.html`,
  `docs.html`) contain hand-written navigation links to that output.
  `\page <id>` produces `htdocs/<id>.html`. `@defgroup <id>` produces
  `htdocs/group__<id>.html`. Maintain the matching module box in
  `doc/website/docs.html` when adding or changing a documented module.

# Assembly

Each module has one module page. That page sorts the whole module: the
main group, every subgroup, and the types that deepen those groups.

A subgroup declares its parent with `@ingroup <ParentGroup>`. The parent
explains the module-wide model. The subgroup documents one reader task
within that model. Place every feature of the module on the module page
as a subgroup or a main-group chapter. Do not give a feature its own
page because it is short, long, or could ship separately. Decide
subgroup versus remaining in the parent by coupling: a subgroup needs
its own reader task or vocabulary. Do not create or merge a subgroup
merely because its documentation is short.

The module page contains no concept, contract, usage rule, or example
that belongs in a group or class comment. It assembles that
documentation with `@copydetails`, not `@copydoc`. The page keeps the
section structure. Copy the main group first, then subgroups, in reader
order. The page defines that order. Open with a short table of contents
naming the main sections that follow. Put the central type of the object
model or reader task where the reading order needs it, even if that is
not inheritance or `main()` order.

Copy a type with `@copydetails` when it deepens the group model and its
detailed description can open as the next chapter beat. Prefer that
continuation. Do not force it. If the class text would reteach the
group, need glue of more than one thought, or cannot stand as the next
beat, keep the class short and keep the content in the group. A brief
is enough when the group already holds the content or the type adds no
new question; do not copy that type onto the page. A short glue
sentence may bridge sections.

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

Dense inventory, do not write:

    A widget supplies identity, geometry, visibility, scaling, event
    dispatch, and pointer capture. Construct widgets only after an
    Application exists.

Packed member tour, do not write:

    Application is the console process root. There is one instance. It
    uses an EventLoop. run() enters that loop. exit() leaves it. loop()
    returns the loop.

Too tight, do not write:

    Every visual Forms object is a Widget. Application is not. A widget
    lives in a parent chain that reaches a Screen; that chain is a
    runtime relationship, not ownership. Geometry is logical: position
    is in parent coordinates, size is local.

Connected prose:

    Every visual Forms object is a Widget. Application is not a widget,
    even though it creates the runtime those widgets use.

    A widget lives in a parent chain that reaches a Screen. That chain
    is a runtime relationship, not ownership, so destroying a parent
    does not destroy its children.

    Geometry is logical rather than device pixels. Position is measured
    in the parent, which means a widget keeps the same local size when
    the parent moves it.

    Application is the console process root. There is one instance per
    process, which is the instance that widgets and windows already
    assume when they are constructed.

    The default constructors create a MainLoop. When the caller passes
    a loop instead, that loop stays with the caller, who remains
    responsible for its lifetime.

```cpp
/** @brief Runtime root of a Forms user interface.

    %MyClass is the process root in the model above. It is not a
    %Widget, and it does not stand in the visual parent chain.

    The code that creates windows and controls keeps them alive.
    Attaching them to the hierarchy does not transfer ownership, so
    destroying the application does not destroy those objects.

    The default constructors create a %MainLoop. When the caller
    passes a loop instead, that loop remains owned by the caller.


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
