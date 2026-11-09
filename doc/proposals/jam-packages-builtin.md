---
proposal: jam-packages.md
---

# Builtin Source Packages

A source package lives under `src/<pkg>/`. It may be a plain directory
or a git submodule. It is never fetched at configure time. When
selected it is built as part of the project and exposes itself through
the common package variables.

A builtin package is also a consumer of the package interface. It may
depend on other packages. Those dependencies are already resolved when
the builtin is configured. The builtin does not care whether a
dependency is itself builtin, prebuilt, or system.

## Timing {#timing}

Package resolution in `Jamconfigure` finishes before any builtin
`SubInclude` runs. Dependencies are resolved before the packages that
need them. For the current set the order is zlib, then png, jpeg,
FreeType, then ssl and crypto.

When `Builtin-Use` includes a package `Jamconfigure`, and later when
the package `Jamfile` is entered, the six variables of every dependency
are already final. The builtin reads them only through the accessors.

## Init {#init}

`builtin.jam` provides:

```
Builtin-Use pkg : root
```

It registers the source root and returns the package name when the
package succeeds, empty otherwise.

Presence is a directory check. There is no network access and no
download. When the root exists the rule includes the package's
`Jamconfigure`. That file performs any checks the package needs.

- If the directory is missing the rule returns empty.
- If the package `Jamconfigure` calls `Exit` the whole configure aborts.
- If the package sets `pkg.TARGET` the rule returns the package name.

When the package is selected, `builtin.jam` issues the `SubInclude`
that descends into the source package's Jamfile. The descent happens
once, after all package resolutions have finished.

## Target {#target}

The source package sets the common variables itself in its own
`Jamconfigure`, and only after its checks have succeeded:

```
Packages-SetIncludes zlib : $(root) ;
Packages-SetTarget   zlib : z ;
```

(`z` or `libz`, whichever name the `Library` or `SharedLibrary` rule
uses.) It does not set the flag variables.

A non-empty `pkg.TARGET` is how `Builtin-Use` and consumers detect a
builtin package. There is no separate status flag and no separate
`WITH_BUILTIN_xxx`.

A source package that is also usable as a standalone project detects
whether it was included from a parent and sets the variables only in
that case. Its own top-level Jamconfigure continues to work when it is
built alone.

## Baseline

`src/Pt/Jamfile`, `src/Pt-Gfx/Jamfile`, `src/Pt-Ssl/Jamfile` and the
inline source Jamfiles apply `LinkLibs` and `LinkFlags` only. They do
not apply `FLibPath` of `LIBDIR`.

## Dependencies {#dependencies}

A builtin package reads its dependencies through the accessors. The
variables are already set. The package applies them with the ordinary
Jam rules.

Example for png that depends on zlib:

```
# src/libpng/Jamfile
SubDirHdrs [ Packages-Includes zlib ] ;
SubDirCcFlags [ Packages-Defines zlib ] ;

Library png : png.c ... ;

if [ Packages-Target zlib ]
{
    LinkLibraries png : [ Packages-Target zlib ] ;
}
else
{
    LinkLibs png : [ Packages-LinkLibs zlib ] ;
    LinkFlags png : [ FLibPath [ Packages-LibDir zlib ] ]
                    [ Packages-LinkFlags zlib ] ;
}

MakeLocate png : $(PT_OUTPUT_TARGET) ;
```

The png Jamfile does not test how zlib was resolved. A non-empty
target means link that target; otherwise apply `LINKLIBS` together with
`FLibPath` of `LIBDIR` and `LINKFLAGS`. The consumer of png links only
png.

Prebuilt packages have no Jamfile, so `Prebuilt-Use` walks
`prebuilts.<pkg>.DEPENDS` for `INCLUDES` and `LIBDIR`. That walk is not
used for builtin packages.

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
