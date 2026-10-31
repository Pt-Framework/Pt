---
proposal: forms-displaylist.md
---

# Commands

A display list is a flat sequence of drawing operations.

## Commands

Each command carries its arguments. Resources are referenced by id.
The sequence is independent of any backend.

## Builder

The builder appends commands. It holds the current transform, clip,
and style until the next change. A change writes a command.

## Replay

Replay plays the sequence on a painter. The painter sees the same
operations the builder recorded.
