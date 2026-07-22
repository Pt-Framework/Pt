# Platinum (Pt) - Agent Instructions
Pt is a C++ framework for high-performance, multi-platform apps from one codebase.

## General Instructions

- Prefer substantive improvements; breaking changes are acceptable when they
  improve the design. Compatibility is desirable but secondary.
- After a completed task, suggest instruction-file fixes if the task revealed
  missing, unneeded, unclear, or incorrect instructions.
- Code only, no explanation. Bullets over paragraphs. No explanations unless asked.
- Prefer repo-root-relative paths; use absolute paths when tools are path-ambiguous.
- Multi-root workspaces may show the same file under different folder roots.
- Do not search `build/`, `tmp/`, `deps/`, `doc/website/htdocs/`, or large
  vendored trees (`src/skia`, `src/openssl`, …) unless the task requires it.

## Task Specific Instructions

Before acting, choose the **smallest** set of modules that match the task
(usually 1–3). Read each chosen file **fully once**. Then act.

- Do **not** load modules “just in case”.
- Do **not** re-read a module already loaded in this session.
- Do **not** skip a module that clearly matches.
- Do **not** invent conventions from memory; the file is authoritative.

### Instructions Index

- Coding guidelines & style, directory layout, writing and reviewing code:
  `.github/instructions/coding.instructions.md`

- Doxygen comments, public headers, API docs:
  `.github/instructions/documenting.instructions.md`

- Build system & setup, building, build configuration, adding sources files:
  `.github/instructions/building.instructions.md`

- Unit tests, test suites, assertions, test registration, test failures:
  `.github/instructions/testing.instructions.md`
  `.github/instructions/pt-unit-api.instructions.md`

- Jam scripts, `Jamfile`, `*.jam` (not jam C sources):
  `.github/instructions/jam.instructions.md`

- Jam executable internals, `jam/src`, Jambase boot, toolset `.jam`:
  `.github/instructions/jam-internals.instructions.md`

- Signals, slots, delegates, events, connect, emit, observable:
  `.github/instructions/pt-api-sigslot.instructions.md`

- Pixel types, image formats, image access, pixel iteration (`Pt::Gfx` images):
  `.github/instructions/pt-gfx-api-images.instructions.md`

- Draw shapes, paths, text, bitmaps, stroke, fill, gradient, paint surface:
  `.github/instructions/pt-gfx-api-drawing.instructions.md`

- Forms UI, styling, widget renderers, layout, measure, `PlatinumStyle`:
  `.github/instructions/pt-forms-dev-styles.instructions.md`
