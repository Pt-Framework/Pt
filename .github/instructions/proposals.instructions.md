---
applyTo: "doc/proposals/**/*.md"
description: "Design proposals that lead implementation"
---

# Overview

`doc/proposals/` holds design proposals that may precede implementation.
A proposal is not API documentation and not a second source of truth
for an implemented surface.

The intention is that architecture and API design can run ahead of
code. Each proposal has exactly one corresponding concept file. Open
implementation steps are collected in one implementation topic per
feature. Once the design is accepted, the concept moves to
`doc/concepts/`. Stable public rules later move into the normal
documentation layers (class chapter, group chapter, or guide page).
The proposal then shrinks to a short decision record that points at
the concept.

## File system structure

- One proposal file per coherent design topic.
- Exactly one concept file per proposal. While the design is open the
  concept may live next to the proposal; once accepted it moves to
  `doc/concepts/`.
- One implementation topic per feature (not per task). It collects the
  open implementation steps as sections or anchors.
- An index file (for example `websocket.md`) stays thin. It links to
  the concept and to the implementation topic, and lists open points
  that point at anchors in the implementation topic.
- File names are lowercase with hyphens. ASCII only.

## Lifecycle

1. A proposal is written while the design is still open. It states the
   model that should become the concept.
2. The proposal is refined until the design is accepted.
3. The concept file is placed in `doc/concepts/` (one concept per
   proposal). The proposal shrinks to a decision record that points at
   the concept.
4. Open implementation steps are recorded in the feature's
   implementation topic. The index links to those anchors.
5. Implementation follows the concept and the implementation topic.
6. Stable public rules move into class chapters, group chapters, or
   guide pages.
7. Finished steps are removed from the implementation topic. The
   concept remains the overarching model.

# Creating proposals

- Start from an index file when the topic already has one. Otherwise
  create a thin index.
- Write the proposal so that it can become the concept: public surface,
  ownership, lifetime, failure handling, and the rules that must hold.
- Keep the proposal focused. Do not grow it into a long narrative of
  every task.
- Begin with a short summary and the local open design points.
- Mark status when useful (`proposed`, `accepted`).
- When the design is accepted, move or derive the concept into
  `doc/concepts/` and replace the proposal body with a short decision
  record plus a link to the concept.
- Create or update the implementation topic for the feature and link
  open implementation steps from the index to anchors in that file.
- Do not restate already implemented behavior. Point at the existing
  class chapter, group chapter, guide page, or concept instead.
- Do not invent conventions. Follow existing framework patterns unless
  the proposal explicitly improves them.

# Working through proposals

- Load the thin index first.
- Then load the concept (from `doc/concepts/` once it has moved) and
  only the implementation-topic sections the current task needs.
- Do not load the whole `doc/proposals/` or `doc/concepts/` tree.
- Treat an accepted concept as the design to implement. Prefer the
  concept over a local minimal change when the local change would
  weaken the model.
- Implement against the concept and the relevant implementation-topic
  anchors. Update the concept only if the implementation reveals a
  necessary design adjustment.
- After the work is done:
  - Move stable public rules into the matching class chapter, group
    chapter, or guide page.
  - Remove or mark finished steps in the implementation topic.
  - Leave the concept in place as the overarching model.
  - Keep the proposal as a short decision record that points at the
    concept.
- For an already implemented surface, prefer the matching instruction
  file, the Doxygen chapters, and the concept over any remaining
  proposal text.
