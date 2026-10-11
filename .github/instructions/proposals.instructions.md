---
applyTo: "doc/proposals/**/*.md"
description: "Design proposals that lead implementation"
---

# Overview

`doc/proposals/` holds design proposals that may precede implementation.
A proposal is not API documentation and not a second source of truth
for an implemented surface.

The intention is that architecture and API design can run ahead of
code. Concepts live only in `doc/concepts/`. They are optional. An
index may link to one or more concepts (its own or others). Open
implementation steps are recorded as anchors in the index. Documents
may be changed at any time. Implemented points are removed from the
proposal and the index once finished. When a proposal is complete it
may be deleted.

## Proposal layout

All proposal files live directly in `doc/proposals/` (no subdirectories).

An index file (for example `websocket.md`) is the entry point for a
feature. It stays thin and contains:

- a short list of related documents (concepts from `doc/concepts/`, and
  any still-active proposal files),
- an Open section whose items are anchors in the index itself (or link
  to anchors in a proposal file).

From the index:

- concepts live only in `doc/concepts/`. A concept is recognized by its
  location and by being linked from the index. Multiple concepts are
  allowed,
- individual proposal files, while they still exist, also live flat in
  `doc/proposals/`.

File names are lowercase with hyphens. ASCII only.

# Workflow

- Start from the index. Create it if it does not exist.
- Add a proposal file in `doc/proposals/` for a coherent design topic
  and link it from the index. Keep it focused on the model (public
  surface, ownership, lifetime, failure handling, rules). Do not let it
  grow into a task narrative.
- When a stable model is worth keeping, write or update a concept in
  `doc/concepts/` and link it from the index. Concepts do not start in
  `doc/proposals/`.
- Record open implementation steps as anchors in the index Open section.
- Load the index first, then only the linked concepts and the proposal
  files the task needs.
- When a concept exists, prefer it over a local minimal change that
  would weaken the model.
- Documents may be revised at any time.
- When an implementation point is finished, remove it from the proposal
  and from the index. Move newly stable public rules into the matching
  class chapter, group chapter, or guide page.
- When a proposal has no remaining open points it may be deleted. The
  index and any concepts stay.
- For an already implemented surface, prefer the matching instruction
  file, the Doxygen chapters, and the linked concepts.
- Do not restate already implemented behavior. Do not invent
  conventions.
