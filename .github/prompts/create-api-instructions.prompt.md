---
name: "create-api-instructions"
description: "Create thematic .instructions.md index files for a Pt module"
argument-hint: "Module name, e.g. Pt-System or Pt-Net"
agent: "agent"
---

Create thematic `.instructions.md` files for the given Pt module so that agents can quickly find the relevant public headers for a task. Use the existing `.github/instructions/pt-api-*.instructions.md` files as a style reference.

## Input

The user provides the module name (e.g. `System`, `Net`, `Http`) and optionally the header directory (default: `include/Pt/<Module>/`).

## Steps

1. **Explore the module**
   - List all public headers in `include/Pt/<Module>/`.
   - Identify any Doxygen group headers named `Api*.h` (e.g. `Api.h`, `Api-Threads.h`).
   - Distinguish real `@defgroup` group headers from Doxygen workaround headers that only contain declarations.

2. **Group thematically**
   - Create one `.instructions.md` file per coherent theme.
   - Every real Doxygen group header must map to exactly one instructions file.
   - Group related non-group headers around the same themes.

3. **Name the files**
   - Use the prefix `pt-<module>-api-` and a short thematic suffix, e.g. `pt-<module>-api-<theme>.instructions.md`.
   - Only the core module uses the `pt-api-` prefix.
   - If a group already has an existing instructions file (e.g. `pt-api-sigslot.instructions.md`), update it instead of creating a duplicate.

4. **Write each file**
   - Start with YAML frontmatter containing `description: "..."`.
   - Use bullet lists in the style of `.github/instructions/pt-gfx-api-drawing.instructions.md`:
     - Each bullet begins with an operational key phrase that tells an agent whether the header is relevant.
     - End the bullet with the header path in backticks, e.g. `include/Pt/System/Thread.h`.
   - Do not include code examples.
   - Do not list source files (`src/...`).

5. **Handle Doxygen groups correctly**
   - Include real `@defgroup` headers in the matching instructions file.
   - Omit Doxygen workaround `Api*.h` headers that have no `@defgroup`.

6. **Update AGENTS.md**
   - Add one index entry per new instructions file in the same style as the existing entries:
     ```
     - For <topic> read:
       `.github/instructions/<filename>.instructions.md`
     ```
   - Place module entries near the other `pt-<module>-api-*` entries.

## Exclusions

- Do not create a file for platform helpers (e.g. `Main.h`, `Pack.h`, `WinVer.h`).
- Do not include headers from other modules.
- Do not include build scripts, source files, or test files.

## Verification

Before finishing, confirm:
- All new files are under `.github/instructions/` and have the `pt-<module>-api-` prefix.
- Each file has a YAML frontmatter with `description`.
- Every real Doxygen group in the module is assigned to one instructions file.
- No workaround `Api*.h` without `@defgroup` is listed.
- `AGENTS.md` references each new file once.
