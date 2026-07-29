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
- Use only ASCII characters in all output (code, comments, file names, messages)
  unless the task explicitly requires non-ASCII characters (e.g. localized strings,
  binary data, or Unicode-specific tests).

## Task Specific Instructions

Before acting, choose the **smallest** set of modules that match the task
(usually 1–3). Read each chosen file **fully once**. Then act.

- Do **not** load modules “just in case”.
- Do **not** re-read a module already loaded in this session.
- Do **not** skip a module that clearly matches.
- Do **not** invent conventions from memory; the file is authoritative.

### Instructions Index

- For Coding guidelines & style, directory layout, writing and reviewing code read:
  `.github/instructions/coding.instructions.md`

- For Doxygen comments, public headers, API docs read:
  `.github/instructions/documenting.instructions.md`

- For Build system & setup, building, build configuration, adding source files read:
  `.github/instructions/building.instructions.md`

- For Unit tests, test suites, assertions, test registration, test failures read:
  `.github/instructions/testing.instructions.md`
  `.github/instructions/pt-unit-api.instructions.md`

- For Jam scripts, `Jamfile`, `*.jam` (not jam C sources) read:
  `.github/instructions/jam.instructions.md`

- For Jam executable internals, `jam/src`, Jambase boot, toolset `.jam` read:
  `.github/instructions/jam-internals.instructions.md`

- For Signals, slots, delegates, events, connect, emit, observable read:
  `.github/instructions/pt-api-sigslot.instructions.md`

- For Pixel types, image formats, image access, pixel iteration (`Pt::Gfx` images) read:
  `.github/instructions/pt-gfx-api-images.instructions.md`

- For Draw shapes, paths, text, bitmaps, stroke, fill, gradient, paint surface read:
  `.github/instructions/pt-gfx-api-drawing.instructions.md`

- For Forms UI, styling, widget renderers, layout, measure, `PlatinumStyle` read:
  `.github/instructions/pt-forms-dev-styles.instructions.md`
