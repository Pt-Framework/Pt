# Description

Platinum (Pt) is a comprehensive C++ framework, which allows developers to
write high-performance applications for many platforms with only one codebase.
It provides a large amount of features and is still very easy to use.

# General Instructions

- At the end of every completed task, suggest improvements to the instruction
  files if the task revealed missing, unneeded, clear, or incorrect instructions.

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

Multi-root workspace: subfolders (e.g. `include/Pt/Gfx`, `src/Pt-Gfx`) are
added alongside the project root ("All Files"), causing overlapping paths that
confuse relative-path-based tool parameters.

- **`grep_search`** `includePattern`: relative paths often fail; prefer the
  absolute project root, a filename glob (`**/Rect.h`), or omit entirely.
- **`file_search`**: works across all folders; duplicate results are expected.
- **`read_file`**: always use absolute paths.
- **`vscode_listCodeUsages`**: `filePath` fails; always use `uri` with an absolute URI.
