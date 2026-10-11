---
applyTo: "doc/proposals/**/*.md"
description: "Design proposals that lead implementation"
---

# Overview

`doc/proposals/` holds design proposals that may precede implementation.
A proposal is not API documentation and not a second source of truth
for an implemented surface.

The intention is that architecture and API design can run ahead of
code. A proposal may have a corresponding concept file in
`doc/concepts/` (optional, at most one). Open implementation steps are
collected in one implementation topic per feature. Documents may be
changed at any time. Implemented points are removed from the proposal
once finished. When a proposal is complete it may be deleted.

## Proposal layout

All proposal files live directly in `doc/proposals/` (no subdirectories).

An index file (for example `websocket.md`) is the entry point for a
feature. It stays thin and contains:

- a short list of related documents (concept if present, implementation
  topic, and any still-active proposal files),
- an Open section whose items link to anchors in the implementation
  topic.

From the index:

- the concept, when it exists, lives in `doc/concepts/` (at most one
  concept file per proposal),
- the implementation topic (one per feature) lives in `doc/proposals/`
  and holds the open implementation steps as sections or anchors,
- individual proposal files, while they still exist, also live flat in
  `doc/proposals/`.

File names are lowercase with hyphens. ASCII only.

# Workflow

- Start from the index. Create it if it does not exist.
- Add a proposal file in `doc/proposals/` for a coherent design topic
  and link it from the index. Keep it focused on the model (public
  surface, ownership, lifetime, failure handling, rules). Do not let it
  grow into a task narrative.
- When a concept is useful, place it in `doc/concepts/` and link it from
  the index.
- Record open implementation steps in the implementation topic and link
  them from the index Open section.
- Load the index first, then only the concept (if any) and the
  implementation-topic sections or proposal files the task needs.
- When a concept exists, prefer it over a local minimal change that
  would weaken the model.
- Documents may be revised at any time.
- When an implementation point is finished, remove it from the proposal
  and the implementation topic. Move newly stable public rules into the
  matching class chapter, group chapter, or guide page.
- When a proposal has no remaining open points it may be deleted. The
  index and any concept stay.
- For an already implemented surface, prefer the matching instruction
  file, the Doxygen chapters, and the concept (if present).
- Do not restate already implemented behavior. Do not invent
  conventions.
