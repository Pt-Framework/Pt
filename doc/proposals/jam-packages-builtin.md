---
proposal: jam-packages.md
---

# Builtin Source Packages

A source package lives under `src/<pkg>/`. It may be a plain directory
or a git submodule. It is never fetched at configure time. When
selected it is built as part of the project and exposes itself through
the common package variables.

## Init {#init}

`Builtin-Use` registers the source root for a package name and, when
called for selection, returns whether the root is present:

```
Builtin-Use zlib : src/zlib
```

Presence is a directory check. There is no network access and no
download. The rule returns the package name when the root exists,
empty otherwise.

When the package is selected, `builtin.jam` issues the `SubInclude`
that descends into the source package's Jamfile. The descent happens
once, after all registrations have been collected.

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

A non-empty `pkg.TARGET` is how consumers detect a builtin package.
There is no separate `WITH_BUILTIN_xxx` flag.

Consumers that see a non-empty `pkg.TARGET` link the target:

```
LinkSharedLibraries mylib : $(zlib.TARGET) ;
```

instead of using `LinkLibs` and `LinkFlags`.

A source package that is also usable as a standalone project detects
whether it was included from a parent and sets the variables only in
that case. Its own top-level Jamconfigure continues to work when it is
built alone.

## Selection {#selection}

Builtin is used in two cases only:

- Override: `--with-inline-xxx` is given and `Builtin-Use` returns the
  package name. This overrides prebuilt and system/custom.
- Fallback: nothing earlier (prebuilt or system/custom) succeeded and
  `Builtin-Use` returns the package name.

Presence of the root alone does not select the package. Without the
option and without a failed earlier step the package is not builtin.

When builtin is selected the package-specific `*-Configure` is not
called. The source package has already set `pkg.INCLUDES` and
`pkg.TARGET`.
