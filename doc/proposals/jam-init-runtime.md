---
proposal: jam-init.md
---

# Jam Initialization Runtime

## Runtime invocation

After initialization, the entry script starts the selected host binary from
`<JAM_ROOT>/bin/`. It selects the platform host ID; macOS always selects
`macos-universal`. It explicitly selects the installed `<JAM_ROOT>/Jambase`.
The runtime does not depend on a vendored `jam/` directory below the project
root.

The project working directory remains the current directory of the entry
script invocation. `Jamconfigure` and `Jamfile` are therefore found in the
project exactly as they are today. The installed runtime supplies the Jam
rules; the project supplies its build configuration and targets.

The mechanism by which the entry script makes `JAM_ROOT` available to the
installed `Jambase` is an implementation detail. Before the installed
`Jambase` loads a Jam module, it derives the library directory from
`JAM_ROOT`.

## Project modules

`Jamlocal` is an optional script in the project root. `Jambase` includes it
once after `Load` and `JamModuleDir` exist and after the snapshot modules
required for boot have loaded, before the phase branch. A missing `Jamlocal`
is not an error. Paths in it are relative to the project root. It is found
by `include`, like `Jamconfigure` and `Jamfile`, and is not a snapshot file.

`Load` remains the single rule for loading Jam runtime files. A relative name
is resolved through `JAM_LIBPATH`. The first entry is `<JAM_ROOT>/lib/`.
`JamModuleDir` is the rule that appends directories behind that entry. It
rejects an empty argument and a path that is not a directory. Comparison is
literal. A directory already on the path is skipped. The rule cannot remove
or replace the snapshot entry. Call order is search order.

A project registers the directories it needs from `Jamlocal`, then loads the
modules those directories provide. `Jambase` remains the sole owner of the
runtime and configure phases. A directory registered in `Jamlocal` is visible
in every phase, because every process reads `Jamlocal` again. A call from
`Jamconfigure` or `Jamfile` applies to the rest of that process only.

For example, `Jamlocal` contains:

```jam
JamModuleDir Jamlib ;
Load company-tools.jam ;
```

`Load` then tests `<JAM_ROOT>/lib/company-tools.jam` and, only if that file
does not exist, `Jamlib/company-tools.jam`. The first existing file is
loaded. A project module supplements the runtime but does not override a
same-named snapshot module. This gives versioned runtime rules priority and
prevents an update from changing snapshot behavior through an unintended
name collision.

If no file exists, the error identifies the requested name and the checked
locations. Future rules that resolve Jam modules use the same path-resolution
behavior rather than introducing a separate extension search mechanism.

## Documentation and migration

The Jam getting-started and usage documentation describes `init`,
`JAM_ROOT`, the three installation sources, the snapshot pin, the
snapshot layout, and the project module search order. It documents
`JAM_DIST_ROOT` and replacing the entry scripts to take a newer
snapshot when distribution initialization is implemented.

Projects start Jam through their entry scripts, which select the
installed `Jambase`. Pt versions `jam/` in this repository.
Other projects copy the entry scripts and install the snapshot from the
distribution.
