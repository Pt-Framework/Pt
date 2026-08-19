# Platinum (Pt) - Agent Instructions

Pt is a general-purpose, easy to use C++ framework for high-performance,
multi-platform applications from one codebase. Develop durable framework
concepts, APIs, and implementations that must serve many applications,
platforms, and user requirements.

## General Instructions

- Treat every change as framework design, not application-specific code. Prefer
  coherent, composable abstractions that fit existing framework concepts and
  serve a broad range of applications, platforms, and requirements.
- Do not default to the smallest local solution when it would create a narrow
  API, encode one application's policy, or prevent valid future uses.
- Evaluate designs beyond the current example: consider API clarity,
  extensibility, cross-platform behavior, performance, ownership, and failure
  handling. Avoid speculative generalization; broader abstractions must remain
  justified, understandable, and proportionate to their boundary.
- Reuse and extend established framework patterns when they express the model
  well. Do not add special cases or convenience shortcuts that weaken
  conceptual consistency.
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
(usually 1–4). Read each chosen file **fully once**. Then act.

- Do **not** load modules “just in case”.
- Do **not** re-read a module already loaded in this session.
- Do **not** skip a module that clearly matches.
- Do **not** invent conventions from memory; the file is authoritative.

### Instructions Index

Read the matching file explicitly whenever its topic applies to the task, even
if it has an `applyTo` pattern that may already auto-attach - never assume its
content is in context without having read it.

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

- For Basic types, Any, dates, times, events and atomic integers read:
  `.github/instructions/pt-api-basics.instructions.md`

- For Unicode strings, text codecs, regex, Base64, parsing and formatting read:
  `.github/instructions/pt-api-text.instructions.md`

- For Memory management, smart pointers and allocators read:
  `.github/instructions/pt-api-memory.instructions.md`

- For Coroutines, awaitables and generators read:
  `.github/instructions/pt-api-coroutines.instructions.md`

- For Serialization, composers and formatters read:
  `.github/instructions/pt-api-serialization.instructions.md`

- For I/O streams, buffers and zlib streams read:
  `.github/instructions/pt-api-streams.instructions.md`

- For Program arguments, settings, type traits and helper utilities read:
  `.github/instructions/pt-api-utilities.instructions.md`

- For Pixel types, image formats, image access, pixel iteration (`Pt::Gfx` images) read:
  `.github/instructions/pt-gfx-api-images.instructions.md`

- For Draw shapes, paths, text, bitmaps, stroke, fill, gradient, paint surface read:
  `.github/instructions/pt-gfx-api-drawing.instructions.md`

- For Forms UI, styling, widget renderers, layout, measure, `PlatinumStyle` read:
  `.github/instructions/pt-forms-dev-styles.instructions.md`
