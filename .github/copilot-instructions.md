# Pt

Pt is a cross-platform C++ framework.

## General Rules
 - Do not add third-party dependencies without discussion

## Visual Code Issues

- **`grep_search` in this Multi-Root Workspace**: Due to overlapping workspace folders, using specific relative paths in `includePattern` (e.g., `include/Pt/Gfx/Rect.h`) often fails to find files unless the file is actively open in the editor.
  - **Workaround 1**: Use only the filename or a generic glob (e.g., `Rect.h` or `**/Rect.h`).
  - **Workaround 2**: Omit `includePattern` entirely and do a global search.
  - **Workaround 3**: If the exact path is known, prefer the `read_file` tool.
