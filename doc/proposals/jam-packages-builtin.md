---
proposal: jam-packages.md
---

# Builtin Source Packages

A source package lives under `src/<pkg>/`. It may be a plain directory
or a git submodule. It is never fetched at configure time. When
selected it is built as part of the project and exposes itself through
the common package variables.

## Init {#init}

`Builtin-Use` registers the source root for a package name:

```
Builtin-Use zlib : src/zlib
```

It checks that the directory exists and records the root. Presence is a
directory check. There is no network access and no download.

When the package is selected, `builtin.jam` issues the `SubInclude`
that descends into the source package's Jamfile. The descent happens
once, after all registrations have been collected.

A source package may itself depend on other source packages. Those
dependencies are declared the same way. The priority rules prevent a
source package from being mixed with a prebuilt package of the same
name.

## Target {#target}

The source package sets the common variables itself, normally in its
own Jamconfigure or in a small registration file that the parent
includes:

```
zlib.INCLUDES = $(PT_TOP)/src/zlib ;
zlib.TARGET   = z ;
```

(`z` or `libz`, whichever name the `Library` or `SharedLibrary` rule
uses.) It does not set the flag variables.

Consumers that see a non-empty `pkg.TARGET` link the target:

```
LinkSharedLibraries mylib : $(zlib.TARGET) ;
```

instead of using `LinkLibs` and `LinkFlags`.

A source package that is also usable as a standalone project detects
whether it was included from a parent and sets the variables only in
that case. Its own top-level Jamconfigure continues to work when it is
built alone.

## Presence and skip

If the registered root exists and no higher-priority source (user or
prebuilt) claimed the package, the package is source. The
package-specific `*-Configure` is skipped and `WITH_BUILTIN_xxx` is
set.

If the root is absent the package falls through to the system probe.
