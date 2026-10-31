---
proposal: jam-packages.md
---

# Package Interface

Source packages and prebuilt packages share one variable interface.
A package name `pkg` exposes these variables:

- `pkg.INCLUDES`
- `pkg.DEFINES`
- `pkg.LINKLIBS`
- `pkg.LIBDIR`
- `pkg.LINKFLAGS`
- `pkg.TARGET`

Prebuilt and system packages fill the flag variables. Source packages
fill `pkg.TARGET` (and usually `pkg.INCLUDES`). Consumers read only
these variables. Package-specific `*-Configure` rules stay ignorant of
whether the values came from a user, a prebuilt bundle, a source tree,
or a system probe.

## Variables {#variables}

The common variables are dotted: `$(pkg.INCLUDES)`, `$(pkg.DEFINES)`,
`$(pkg.LINKLIBS)`, `$(pkg.LIBDIR)`, `$(pkg.LINKFLAGS)`, `$(pkg.TARGET)`.

`pkg.TARGET` is the Jam target name of a source-built library. When it
is set, linking uses that target instead of `pkg.LINKLIBS` and
`pkg.LINKFLAGS`.

The flag variables hold the tokens for includes, defines, link libraries,
library directories and link flags. Empty lists are omitted.

Consumers, user overrides and Configure rules use only the dotted form.
The former five-tuple names (`ZLIB_*`, `PNG_*`, `JPEG_*`, `FREETYPE_*`,
`SSL_*`) are not produced and are not read.

`packages.jam` provides accessors so callers do not spell the dotted
names by hand:

```
Packages-Includes pkg
Packages-Defines pkg
Packages-LinkLibs pkg
Packages-LibDir pkg
Packages-LinkFlags pkg
Packages-Target pkg

Packages-SetIncludes pkg : values *
Packages-SetDefines pkg : values *
Packages-SetLinkLibs pkg : values *
Packages-SetLibDir pkg : values *
Packages-SetLinkFlags pkg : values *
Packages-SetTarget pkg : value
```

Getters return the current list (empty if unset). Setters replace the
list. Direct assignment to the dotted variables remains valid; the
accessors are the preferred form.

## Modules {#modules}

Three modules own the mechanism:

- `packages.jam` defines the common variable interface and its
  accessors. It knows nothing about prebuilt bundles, source package
  roots, configure options, or resolution order.
- `prebuilt.jam` maps a prebuilt bundle onto the common variables. It
  loads the bundle manifest.
- `builtin.jam` registers source-package roots under `src/`, checks
  presence, and descends with `SubInclude` when a source package is
  selected.

`packages.jam` is included once. The other two modules are loaded on
demand by `Jamconfigure`.

## Configure {#configure}

Resolution order and Configure dispatch live in `Jamconfigure`. It is
the only place that knows about user settings, configure options,
prebuilt and builtin.

Resolution order for each package is fixed and admits no mixing:

1. Explicit user settings (`-s pkg.INCLUDES=...` and the other dotted
   variables on the command line or in the environment).
2. Source package, when a configure option that selects it is given
   (for example `--with-inline-xxx`) and a registered source root is
   present.
3. Prebuilt, when `--with-prebuilt` selected a bundle and the package is
   listed in it.
4. System probe via the package-specific `*-Configure`.

A package is either user-supplied, source, prebuilt, or system. User
values win over all other sources. Source selection is opt-in via
configure options; the mere presence of a source root does not select
it.

When a source package is selected the package-specific `*-Configure`
is not called. The source package has already set `pkg.INCLUDES` and
`pkg.TARGET`.

When prebuilt or system is used, `*-Configure` is called with the
values already filled (prebuilt) or empty (system). The Configure rule
probes only when its arguments are empty; otherwise it verifies the
supplied values.

`WITH_BUILTIN_xxx` and `WITH_xxx` keep their current meaning for
consumers.
