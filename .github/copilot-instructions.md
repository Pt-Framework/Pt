# Description

Platinum (Pt) is a comprehensive C++ framework, which allows developers to
write high-performance applications for many platforms with only one codebase.
It provides a large amount of features and is still very easy to use.

# General Instructions

- Do not add third-party dependencies without discussion.
- At the end of every completed task, suggest improvements to the instruction
  files if the task revealed missing, unclear, or incorrect instructions.

# Task Specific Instructions

Before performing any task, identify which of the instruction files below are
relevant to your task. You MUST read each relevant instruction file completely
before you perform the task. Do NOT skip reading these files, even if you
think you have all relevant information.

- Write code, review code, code organization, naming, style:
  `.github/instructions/coding.instructions.md`
- Doxygen comments, document public headers, API docs:
  `.github/instructions/documenting.instructions.md`
- Configure, build, clean, verify code changes, jam build:
  `.github/instructions/building.instructions.md`
- Write tests, run tests, add test methods, debug test failures,
  test suites, assertions, test registration:
  `.github/instructions/testing.instructions.md`
  `.github/instructions/pt-unit-api.instructions.md`
- Jam scripts, Jamfile, .jam files:
  `.github/instructions/jam.instructions.md`
- Jam executable internals, jam C code, Jambase boot, toolset .jam:
  `.github/instructions/jam-internals.instructions.md`
- Signals, slots, delegates, events, connect, emit, observable:
  `.github/instructions/pt-api-sigslot.instructions.md`
- Pixel types, image formats, image access, pixel iteration:
  `.github/instructions/pt-gfx-api-images.instructions.md`
- Draw shapes, paths, text, bitmaps, stroke, fill, gradient,
  transform, blend mode, font, paint surface:
  `.github/instructions/pt-gfx-api-drawing.instructions.md`

# Visual Code Issues

This is a multi-root workspace where subfolders (e.g. `include/Pt/Gfx`,
`src/Pt-Gfx`) are added as separate workspace folders alongside the
project root ("All Files"). This causes overlapping paths that confuse
relative-path-based tool parameters.

- **`grep_search`**: Relative paths in `includePattern` (e.g.
  `include/Pt/Gfx/Rect.h`) often fail because they resolve against
  the wrong workspace folder.
  - **Preferred**: Use the absolute project root as `includePattern`.
  - **Alternative**: Use only the filename (`Rect.h` or `**/Rect.h`).
  - **Alternative**: Omit `includePattern` entirely for a global search.

- **`file_search`**: Glob patterns work across all workspace folders.
  Results may appear duplicated — this is expected and harmless.

- **`read_file`**: Always use absolute paths. This is the most reliable
  tool when the exact file path is known.
