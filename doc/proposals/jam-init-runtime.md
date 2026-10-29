---
proposal: jam-init.md
---

# Jam Initialization Runtime

## Runtime invocation

After initialization, the entry script starts the selected host binary from
`<JAM_ROOT>/core/bin/`. It selects the platform host ID; macOS always selects
`macos-universal`. It explicitly selects the installed `<JAM_ROOT>/core/Jambase`.
The runtime does not depend on a vendored `jam/` directory below the project
root.

The project working directory remains the current directory of the entry
script invocation. `Jamconfigure` and `Jamfile` are therefore found in the
project exactly as they are today. The installed runtime supplies the Jam
rules; the project supplies its build configuration and targets.

The mechanism by which the entry script makes `JAM_ROOT` available to the
installed `Jambase` is an implementation detail. Before the installed
`Jambase` loads a Jam module, it derives the core library directory from
`JAM_ROOT`.

## Local extensions

`<JAM_ROOT>/local/` is the permanent extension directory. It is outside the
update-managed `core/` tree and may contain:

```text
<JAM_ROOT>/local/
  lib/
    *.jam
```

`Load` remains the single rule for loading Jam runtime files. A relative name
is resolved first below `<JAM_ROOT>/core/lib/` and, only if the core file does
not exist, below `<JAM_ROOT>/local/lib/`.

A project loads the extensions it needs from its `Jamconfigure`. The core
`Jambase` remains the sole owner of the runtime and configure phases. This
keeps each project's module dependencies explicit while allowing several
projects to use the same local extension installation.

For example:

```jam
Load company-tools.jam ;
```

first tests:

```text
<JAM_ROOT>/core/lib/company-tools.jam
<JAM_ROOT>/local/lib/company-tools.jam
```

The first existing file is loaded. A local file supplements the runtime but
does not override a same-named core file. This gives versioned runtime rules
priority and prevents an update from changing core behavior through an
unintended local name collision.

`Load` accepts only relative module names within one of these roots. It rejects
absolute names and parent-directory traversal. If no file exists, its error
identifies the requested name and both checked locations. Future rules that
resolve Jam modules use the same path-resolution behavior rather than
introducing a separate extension search mechanism.

## Documentation and migration

The Jam getting-started and usage documentation describes the phase-1
`--init` command, `JAM_ROOT`, the `core/` and `local/` ownership boundary, and
the local module search order. It documents `JAM_DIST_ROOT` and `--update`
when distribution initialization is implemented.

Projects start Jam through their entry scripts, which select the installed
core `Jambase`. Newly initialized projects need only the entry scripts,
`Jamconfigure`, and `Jamfile`; after distribution initialization they do not
need a versioned `jam/core/` directory.

## Baseline

The current `Load` rule builds one path below `JAMDIR` and does not search a
user extension directory. The documentation still describes the project-root
bootstrap layout.
