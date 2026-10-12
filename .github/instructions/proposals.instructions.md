---
description: "Design proposals that lead implementation"
---

# Overview

A proposal is a set of proposal files with one index file. It lets
architecture and API design run ahead of implementation and records the
current state of that implementation. It is not API documentation and
not a second source of truth for an implemented surface. A proposal may
refer to one or more implementation-neutral concepts.

# Layout

All proposal files live directly in `doc/proposals/` (no subdirectories).

A proposal is a set of proposal files headed by an index file (for
example `websocket.md`). The index is the entry point. It stays thin
and contains:

- a Documents section that lists the other proposal files of this
  proposal and any concepts,
- an Open section. Each item links to a heading in a proposal file and
  states the implementation task,
- an optional Priority section that orders those tasks.

From the index:

- concepts live only in `doc/concepts/`. A concept is recognized by its
  location and by being linked from the index. Several indexes may link
  the same concept,
- the other proposal files of the set also live flat in `doc/proposals/`.

File names are lowercase with hyphens. ASCII only.

The index has no frontmatter. Proposal files carry frontmatter that names
the proposal they belong to:

```yaml
---
proposal: websocket.md
---
```

# Contents

The set records the current state of the implementation against the
model.

The index lists that state. Each Open item is one implementation task
the code does not follow yet. It links to the heading that states the
model and says what to implement.

A proposal file holds one design topic: the public surface, ownership,
lifetime, failure handling, and rules. Where the implementation still
differs, a Baseline section states that behavior.

A concept holds the stable, implementation-neutral model. It has no
Baseline and no Open items.

# Workflow

- Search `doc/proposals/` and `doc/concepts/` before creating an index,
  a proposal file, or a concept. Extend the matching set.
- Add a proposal file in `doc/proposals/` for a coherent design topic,
  set its `proposal` frontmatter, and link it from the index Documents
  section. Keep it focused on the model (public surface, ownership,
  lifetime, failure handling, rules, etc...). Do not let it grow into a task
  narrative.
- When a stable, implementation-neutral model is worth keeping, write
  or update a concept in `doc/concepts/` and link it from the index. A
  concept states the model, not the implementation steps.
- Record an implementation task as an item in the index Open section.
  Link it to the heading that states the model.
- Load the index first, then only the linked concepts and the proposal
  files the task needs.
- When a concept exists, prefer it over a local minimal change that
  would weaken the model.
- Documents may be revised at any time.
- When an Open task is implemented, remove the item from the index.
  Leave the model in the proposal file or fold it into the concept.
  Move a public rule into the matching class chapter, group chapter,
  or guide page. Remove a Baseline once the implementation matches it.
- When a proposal has no remaining open tasks its proposal files may
  be deleted. The index may stay or be removed with them. Any concepts
  stay.
- While Open tasks remain, the proposal states the work. Instruction
  files and Doxygen chapters state the current surface.
- Do not copy behavior that already matches the model. A Baseline
  states only behavior an Open task still changes.
