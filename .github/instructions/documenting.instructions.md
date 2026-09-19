---
applyTo: "**/*.{h,md,page}"
description: "API Documentation"
---

# Layers of Documentation

Documentation is built in four layers. Each layer has its own
formatting, content, structure, and style. Later layers assemble or
link earlier ones; they do not rewrite them.

1. **API declaration comments** — Doxygen comments on member functions,
   parameters, enumerators, and similar declarations. Compact reference.
2. **Group and class detailed descriptions** — the chapters. Textbook
   prose in group files and on class declarations.
3. **Pages** — one module page that copies those chapters into reading
   order, plus guide pages with original prose.
4. **Website** — hand-written navigation to the HTML Doxygen generates.

Write for a reader who uses the public API, not for its implementer.
Compact wording belongs in API declaration comments. Group and class
detailed descriptions are textbook chapters.

The chat convention of code and bullets, and the coding-guideline
preference for short comments, do not apply to Doxygen comments.
Doxygen comments are documentation.

# API Declaration Comments

Doxygen comments on public declarations are the foundation. Every later
layer depends on them.

## Formatting

- Use `/** ... */` block comments. Place the closing `*/` on the next
  line. Do not use leading asterisks on intermediate lines.
- Place `@brief` on the first line. After a blank line, indent further
  text to align with `@brief` (4 spaces from `/**`).
- Separate later Doxygen commands (`@param`, `@return`, `@throw`,
  `@ingroup`) from the brief or detailed text with a blank line.
- Refer to parameters with `@a <name>`.
- Escape class names, namespace-qualified names, and function names in
  prose with `%` unless an explicit Doxygen link is desired
  (`%MyClass`, `%MyClass::begin()`).
- Use `@related <ClassName>` for operators and free functions when
  appropriate.
- Do not use `@class` when the context is already clear to Doxygen.
- Do not document forward declarations.

## Content

Document every public namespace, class, function, enum, and public
member in the public header, not in a `.cpp` file. Internal helpers in
`.cpp` files may use brief comments but do not need Doxygen markup.

A one-line `@brief` without details is enough when the declaration and brief
suffice. Add detail, `@param`, and `@return` only for non-obvious behavior,
contracts, errors, or complex usage. Document ownership, lifetime, side
effects, errors, and ordering only when they are not evident from the
declaration.

Do not narrate internal execution steps. Do not walk through other
members the brief already covers.

## Structure

These comments live next to the declaration they document, in the
public header. Assign the API to a group with `@ingroup` when it has a
meaningful role in that group's reader task. An API may belong to more
than one group. Do not assign an API to a group solely to classify it;
it may remain outside a feature group.

## Style

Compact and precise. A reader scans these comments. They are not
chapters. Brief-style rules apply only here; they do not apply to group
or class detailed descriptions.

Name the result or effect first. Use "Returns ..." for queries,
"Sets ..." or "Changes ..." for mutators, and "Creates ..." or
"Adds ..." for factory and registration operations. Do not begin a
brief with "This function" or repeat the method name in prose.

```cpp
/** @brief Divides @a value by @a divisor.

    @throw %std::invalid_argument if @a divisor is 0.
*/
float divide(float value, float divisor);

/** @brief Returns the application-unique ID of this live widget.
*/
std:size_t id() const;
```

# Group and Class Detailed Descriptions

These texts are the chapters. The group comment is the feature chapter.
The class comment is the type chapter. Pages copy them; the website
links to them. Write the chapters here, not on the page.

## Formatting

Use the same Doxygen block layout as API declaration comments:
`/** ... */`, `@brief` on the first line, detailed description indented
to align with `@brief`, later commands separated by a blank line, `%`
to escape names, no leading asterisks, no `@class` when the context is
clear.

The module group header (`Api-<Module>.h`) declares groups with
`@defgroup`. Each feature file `Api-<Feature>.h` is one `@addtogroup`
block with that feature's chapter. A class chapter is the detailed
description on the class declaration, with `@ingroup` for each group
in which the type has a role.

Use `@code` for examples. Use `@par` to title a subtopic in a long
chapter (ownership, geometry, events). Do not use `@section` or
`@subsection` inside a group or class comment; those commands belong on
pages.

## Content

Write a chapter the reader can learn from without scanning every member.
Explain what the feature or type is for, the object model, how the types
work together, how a caller uses them, ownership, lifetime, errors,
ordering, and the distinctions that prevent mistakes. Include the
context needed to use the API correctly, including an API technique that
is part of the contract (C linkage of an export, a cast the platform
requires, matching allocators).

A restatement of members in declaration order is not a chapter. Fold
member facts into the model and leave compact wording to the briefs. A
chapter that only names capabilities is unfinished. A chapter shorter
than a careful chat explanation of the same topic is unfinished.

Use `@code` where the example belongs, and write the sentences around it
that tell the reader what to look at. Teaching the concepts of this
API is the job of the chapter. Teaching C++ itself is not.

Lists, numbered steps, and `@par` subtopics are welcome when they help
the reader choose among alternatives, follow a usage sequence, or open a
new aspect of the same chapter. They supplement the prose; they do not
replace it.

### Groups

The group detailed description is the essential chapter of the feature:
what it is for, then the object model in reading order. Tell the model
once at this level. Shared concepts and shared contracts belong once, in
the group or on the first type that owns the mechanism. Local concepts
belong on the owning class.

A group example shows the group model, not one type. A group needs no
`@code` when prose is enough or the examples live on the classes. Do not
close the group with an example that is only about one type.

Module-level concepts belong in the `@namespace` comment in the module's
`Api.h`.

### Classes

Each class chapter answers what this type is in the model the group
told. Open with a hinge that places the type in that model, then develop
this type: its role, its distinct contracts, how a caller uses it, and
the mistakes that are specific to it. A type-specific example belongs on
the class that owns that remaining question.

The class page must still read as a coherent excerpt, so introduce the
type fully enough that a reader who opened only the class HTML
understands it. Do not paste the group chapter into the class. Overlap
that the class page needs is expected.

Do not shrink the class to a brief because the group exists. Keep a type
to a brief only when it really adds no question of its own: a small
helper, a pure alias, or a type whose whole meaning is already the
group's.

End so the next type on the page is expected when the page copies it.

## Structure

Organize groups around reader tasks, concepts, and public mechanisms,
not inventories of headers or types. A subgroup declares its parent with
`@ingroup <ParentGroup>`. The parent is the module-wide model. The
subgroup is one reader task within that model. A subgroup needs its own
reader task or vocabulary. Do not create or merge a subgroup merely
because its documentation is short.

- Group IDs replace `::` with `-`: `Ns::` -> `Ns-<Feature>`,
  `Ns::Sub::` -> `Ns-Sub-<Feature>`.
- The module group header (`Api-<Module>.h`) owns `@defgroup`. It
  contains the module chapter, then one `@defgroup`
  stub per child with `@ingroup <ModuleGroup>`, in the order those
  children should appear in Doxygen's Modules tree. That order is the
  reason `@defgroup` lives here and not in the feature files.
- Feature chapters live in `Api-<Feature>.h` in the module's public
  include directory. Each file contains one `@addtogroup` block, wrapped
  in include guards (`#ifndef <PROJECT>[_MODULE]_API_FEATURE_H`). Do not
  put `@defgroup` in `Api-<Feature>.h`; that would lose the order
  declared in the module group header.
- Class chapters live on the class in the public header. For typedefs or
  template specializations that Doxygen documents poorly, put class
  documentation in `Api-<ClassName>.h` next to the real header.

## Style

Textbook prose: comprehensive, patient, and as thorough as a tutor
explaining the feature in chat. Complete thoughts, enough context, and
as many paragraphs as the topic needs. This is not a brief and not a
longer brief. "Returns ..." openings and other API-declaration style
rules do not apply here.

Write developed paragraphs. A paragraph may name several types,
relations, or rules when they belong to the same explanation.
Subordinate clauses are wanted when they clarify the contract.

```cpp
/** @defgroup Ns-MyModule Module Name

    @brief Module chapter.

    ...
*/

/** @defgroup Ns-MyFeature Feature Name

    @ingroup Ns-MyModule
*/
```

```cpp
/** @addtogroup Ns-MyFeature

    @brief Feature chapter.

    ...

    @code
    ...
    @endcode
*/
```

```cpp
/** @brief Type chapter.

    ...

    @par ...
    ...

    @ingroup Ns-MyFeature
*/
class MyClass
{
};
```

# Pages

Each module has one module page. That page sorts the whole module: the
main group, every subgroup, and the types that deepen those groups.
Guide pages are separate documents with original prose.

## Formatting

- A page is a Doxygen page comment: `\page <id>` or `@page <id>`.
- Assemble group and class chapters with `@copydetails`, not `@copydoc`.
- Use `@section` for subgroups and direct main-group chapters. Use
  `@subsection` for types below a subgroup.
- Reference a group from elsewhere with the section anchor that holds it
  (`@ref <Page>-<Section>`), not a group-only page ID.
- Guide pages use `@code` for commands and `@verbatim` for directory
  trees and URLs.

## Content

The module page contains no concept, contract, usage rule, or example
that belongs in a group or class comment. It assembles those chapters.
A short glue sentence may bridge sections. If the glue would have to
teach a concept, put that concept in the group or class comment instead.

Open with a short table of contents naming the main sections that follow.
Name each chapter after the reader-facing role, not after internal type
lists or service names.

Guide pages must not repeat API reference that already lives in a group,
or content that already lives on another page; point to it with `@ref`
instead.

## Structure

Pages live in `doc/pages/`. File names are lowercase. A module page ID
uses a `-Page` suffix (`Ns-MyModule` -> `Ns-MyModule-Page`). Section
anchors use the page ID as prefix (`Ns-MyModule-Page-MyFeature`).

Copy the main group first, then subgroups, in reader order. The page
defines that order. Put the central type of the object model or reader
task where the reading order needs it, even if that is not inheritance
or `main()` order.

Place every feature of the module on the module page as a subgroup or a
main-group chapter. Do not give a feature its own page because it is
short, long, or could ship separately.

Copy a type with `@copydetails` when it deepens that section's model.
Prefer copying the central types of a reader task. A brief-only class
that adds no chapter of its own is not copied.

A group keeps its `@ingroup` identity on the module page.

## Style

The module page is assembly, not authorship. The copied group and class
chapters already have the textbook voice. Do not rewrite them on the
page.

Guide pages (`jam-*.page`, `installing.page`, `tutorial.page`, ...)
contain original prose and follow the chapter voice of group and class
detailed descriptions.

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

# Website

The website is navigation to generated HTML. It is not a place to write
API chapters.

## Formatting

Doxygen generates HTML into `doc/website/htdocs/`. Do not edit those
files by hand. The website root files in `doc/website/` (`index.html`,
`docs.html`) are ordinary HTML with hand-written navigation links to
that output.

## Content

- `\page <id>` produces `htdocs/<id>.html`.
- `@defgroup <id>` produces `htdocs/group__<id>.html`.

Maintain the matching module box in `doc/website/docs.html` when adding
or changing a documented module.

## Structure

Hand-written files live in `doc/website/`. Generated files live in
`doc/website/htdocs/`. A new documented module needs a box in
`docs.html` that points at its generated page.

## Style

Navigation labels match the reader-facing names used on pages. Do not
introduce new API explanations on the website; the chapters already
live in groups, classes, and pages.

# Agent Instruction Files

One `.instructions.md` file per high-level feature set, covering one or
more related groups. These files map features and tasks to the relevant
headers and `Api-<Feature>.h` group files. They do not contain
documentation or explanations; those belong in headers and group files.
