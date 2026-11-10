---
proposal: jam-vswhere.md
---

# Jam vswhere Locator

## Purpose

Windows toolset detection uses `vswhere.exe` to locate Visual Studio installations. The tool ships with the Visual Studio Installer. The Jam snapshot must not vendor a copy of it.

## System locator {#system-locator}

`VSWHERE` is the absolute path of the first existing `vswhere.exe` found in this order:

1. `%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe`
2. `%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe` (32-bit systems)
3. `vswhere.exe` on `PATH`

If none exists, detection fails with an error that names the missing Installer path. The error is the same one `jam.bat` already emits when it cannot bootstrap a missing host binary.

`nt.jam` and `vcbase.jam` call this `VSWHERE` with `-products *` so Build Tools are included. Older toolsets (vc14 and earlier) continue to use the existing `VSxxCOMNTOOLS` environment variables.

The snapshot does not contain `lib/vswhere.exe`. `jam/core/meta/contents` omits it. Documentation that lists the binary as a runtime asset is updated.

## Baseline

`jam/core/lib/vswhere.jam` sets `VSWHERE` to the vendored `jam/core/lib/vswhere.exe`. `jam/core/meta/contents` lists that binary. `jam.bat` already locates the Installer copy to build a missing host binary.
