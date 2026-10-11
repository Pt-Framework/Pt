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

# Creating proposals

- Start from the index file of the feature. Create it if it does not
  exist yet.
- Add a proposal file directly in `doc/proposals/` for a coherent
  design topic. Link it from the index.
- Write the proposal so that its stable model can become a concept if
  one is useful: public surface, ownership, lifetime, failure handling,
  and the rules that must hold.
- Keep each proposal focused. Do not let it grow into a long narrative
  of every task.
- When a concept is useful, place or update it in `doc/concepts/` and
  link it from the index.
- Record open implementation steps in the feature's implementation
  topic. Link them from the Open section of the index.
- Do not restate already implemented behavior. Point at the existing
  class chapter, group chapter, guide page, or concept instead.
- Do not invent conventions. Follow existing framework patterns unless
  the proposal explicitly improves them.

# Working through proposals

- Load the index first.
- Then load the concept (if it exists) and only the implementation-topic
  sections (or proposal files) the current task needs.
- Do not load the whole `doc/proposals/` or `doc/concepts/` tree.
- When a concept exists, treat it as the design to implement against.
  Prefer it over a local minimal change when the local change would
  weaken the model.
- Documents may be revised at any time. Update the proposal, concept,
  or implementation topic when the work reveals a necessary adjustment.
- When an implementation point is finished:
  - remove it from the proposal and from the implementation topic,
  - move any newly stable public rules into the matching class chapter,
    group chapter, or guide page,
  - leave the concept in place if one exists.
- When a proposal has no remaining open points it may be deleted. The
  index and any concept stay.
- For an already implemented surface, prefer the matching instruction
  file, the Doxygen chapters, and the concept (if present) over any
  remaining proposal text.
