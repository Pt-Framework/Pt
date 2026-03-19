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
relevant to your task. Read each relevant instruction file completely before
you perform the task.

- **Building**: Building the Pt project with jam.
  Read this when asked about configuring, building, or cleaning the project.
  -> `.github/instructions/building.instructions.md`

- **Coding Style**: Coding style and conventions for C++ files in the Pt project.
  Read this when writing or reviewing C++ code.
  -> `.github/instructions/coding-style.instructions.md`

- **Unit Testing**: Writing and structuring unit tests with the Pt::Unit framework.
  Read this when asked to create tests, add test methods, or debug test failures.
  -> `.github/instructions/testing.instructions.md`

# Visual Code Issues

- **`grep_search` in this Multi-Root Workspace**: Due to overlapping workspace
  folders, using specific relative paths in `includePattern` (e.g.,
  `include/Pt/Gfx/Rect.h`) often fails to find files unless the file is actively
   open in the editor.
  - **Workaround 1**: Use only the filename or a generic glob (e.g., `Rect.h` or `**/Rect.h`).
  - **Workaround 2**: Omit `includePattern` entirely and do a global search.
  - **Workaround 3**: If the exact path is known, prefer the `read_file` tool.
