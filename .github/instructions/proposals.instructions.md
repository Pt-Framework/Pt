---
applyTo: "doc/proposals/**/*.md"
description: "Design proposals that lead implementation"
---

# Overview

A proposal is a set of proposal files with one index file. It holds
design that may precede implementation and records the current state of
that implementation. It is not API documentation and not a second
source of truth for an implemented surface.

The intention is that architecture and API design can run ahead of
code. Stable models may be captured as concepts.

## Proposal layout

All proposal files live directly in `doc/proposals/` (no subdirectories).

A proposal is a set of proposal files headed by an index file (for
example `websocket.md`). The index is the entry point. It stays thin
and contains:

- a short list of related documents (concepts from `doc/concepts/`, and
  the other proposal files that belong to this proposal),
- an Open section whose items are anchors in the index itself (or link
  to anchors in a proposal file).

From the index:

- concepts live only in `doc/concepts/`. A concept is recognized by its
  location and by being linked from the index. Multiple concepts are
  allowed,
- the other proposal files of the set also live flat in
  `doc/proposals/`.

File names are lowercase with hyphens. ASCII only.

# Workflow

- Start from the index of the proposal. Create it if it does not exist.
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
  files and from the index. Move newly stable public rules into the
  matching class chapter, group chapter, or guide page.
- When a proposal has no remaining open points its proposal files may
  be deleted. The index may stay or be removed with them. Any concepts
  stay.
- For an already implemented surface, prefer the matching instruction
  file, the Doxygen chapters, and the linked concepts.
- Do not restate already implemented behavior. Do not invent
  conventions.
