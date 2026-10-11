---
applyTo: "doc/proposals/**/*.md"
description: "Design proposals that lead implementation"
---

# Overview

`doc/proposals/` holds design proposals that may precede implementation.
A proposal is not API documentation and not a second source of truth
for an implemented surface.

The intention is that architecture and API design can run ahead of
code. Open points stay visible and linked. Once a design is accepted
and implemented, its public rules move into the normal documentation
layers (class chapter, group chapter, or guide page). The proposal
then shrinks or becomes a short decision record that points at the
place the rule now lives.

## File system structure

- One coherent topic per file. Prefer many small files over few large
  ones.
- An index file (for example `websocket.md`) stays thin. It lists the
  related proposal files and the open points, with links.
- Related proposals may share a common prefix or live in a
  subdirectory when the set grows.
- File names are lowercase with hyphens. ASCII only.

## Lifecycle

1. A proposal is written while the design is still open.
2. The proposal is refined until the design is accepted.
3. Implementation follows the accepted proposal.
4. Stable public rules move into class chapters, group chapters, or
   guide pages.
5. The proposal is updated: detailed text is replaced by a pointer,
   or the file becomes a short decision record with status
   `implemented`.
6. Remaining open points stay in the proposal (or move to a new one).

# Creating proposals

- Start from an index file when the topic already has one. Otherwise
  create a thin index that will hold the open points and links.
- Write one small file per coherent topic. Do not grow a single file
  into a long narrative.
- Begin each proposal with a short summary and the local open points.
- State the design that should lead the implementation: public
  surface, ownership, lifetime, failure handling, and the rules that
  must hold.
- Mark status when useful (`proposed`, `accepted`).
- Link open points from the index to the concrete proposal file (or
  anchor).
- Do not restate already implemented behavior. Point at the existing
  class chapter, group chapter, or guide page instead.
- Do not invent conventions. Follow existing framework patterns unless
  the proposal explicitly improves them.

# Working through proposals

- Load the thin index first.
- Then load only the one to three proposal files the current task
  needs. Do not load the whole `doc/proposals/` tree.
- Treat an accepted proposal as the design to implement. Prefer the
  design over a local minimal change when the local change would
  weaken the model.
- Implement against the proposal. Update the proposal if the
  implementation reveals a necessary design adjustment.
- After the work is done:
  - Move stable public rules into the matching class chapter, group
    chapter, or guide page.
  - Replace the corresponding proposal text with a pointer to that
    place, or turn the file into a short decision record.
  - Leave only the still-open points in the proposal.
- For an already implemented surface, prefer the matching instruction
  file and the Doxygen chapters over any remaining proposal text.
- After a completed task, update the proposal so it no longer
  duplicates the normal documentation.
