---
description: "Thematic instruction indexes that map topics to public headers."
---

# Registration

- Each instruction file is one topic and is registered once in `AGENTS.md`.
- The entry lists the tasks and features that should trigger the agent to read the file.
- Place entries for the same module or area together.

# Index files

- One instruction file per coherent topic.
- The file maps its tasks and features to the relevant public headers and group files.
- Update an existing file instead of creating a duplicate.

# Naming

- Consistent short prefix for the module or area, followed by a thematic suffix.

# Content

- Indexes only. No documentation, explanations, or examples.
- Bullet lists only.
- Each bullet starts with a task or feature phrase that tells an agent whether the header is relevant.
- End each bullet with the header path in backticks.
- Include every group header that declares or documents a feature set.
- Omit group headers that contain neither a declaration nor documentation.
- Do not list source files, build scripts, test files, or platform helpers.
