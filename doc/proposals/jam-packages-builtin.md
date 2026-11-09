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
dependency is itself builtin, prebuilt, or system. As a provider it
publishes complete public usage on the six variables, the same result
a consumer would get from pkgconf.

## Timing {#timing}

Package resolution in `Jamconfigure` finishes before any builtin
`SubInclude` runs. Dependencies are resolved before the packages that
need them. For the current set the order is zlib, then png, jpeg,
FreeType, then openssl.

When `Builtin-Use` includes a package `Jamconfigure`, and later when
the package `Jamfile` is entered, the six variables of every dependency
are already final. The `Jamconfigure` reads them through the accessors
to publish this package. The `Jamfile` reads them through the accessors
to compile.

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
uses.) `Packages-SetTarget` sets this package's library name.

A non-empty `pkg.TARGET` is how `Builtin-Use` and consumers detect a
builtin package. There is no separate status flag and no separate
`WITH_BUILTIN_xxx`.

A package `Jamconfigure` under `src/<pkg>/` is the builtin configure.
It sets the variables whenever it runs. A later standalone project, if
one is added, detects that it was not included from a parent and leaves
the variables unset.

## Dependencies {#dependencies}

The source package `Jamconfigure` publishes complete public usage.
It sets this package's `INCLUDES` and `TARGET`, then copies each
dependency onto the flag variables. When a dependency is itself
builtin, it appends that dependency's `TARGET` so `LinkLibraries`
receives every Jam library the consumer needs. `Packages-SetTarget`
still sets this package's own name.

The package `Jamfile` consumes dependencies through the accessors to
compile. A static `Library` does not link.

Example for png that depends on zlib:

```
# src/libpng/Jamconfigure
Packages-SetIncludes png : $(root) [ Packages-Includes zlib ] ;
Packages-SetDefines  png : [ Packages-Defines zlib ] ;
Packages-SetTarget   png : png ;
Packages-SetLinkLibs png : [ Packages-LinkLibs zlib ] ;
Packages-SetLibDir   png : [ Packages-LibDir zlib ] ;
Packages-SetLinkFlags png : [ Packages-LinkFlags zlib ] ;

if [ Packages-Target zlib ]
{
    png.TARGET += [ Packages-Target zlib ] ;
}

# src/libpng/Jamfile
if [ Packages-Includes zlib ]
{
    SubDirHdrs [ Packages-Includes zlib ] ;
}

if [ Packages-Defines zlib ]
{
    SubDirCcFlags [ Packages-Defines zlib ] ;
}

Library png : png.c ... ;

MakeLocate png : $(PT_OUTPUT_TARGET) ;
```

The png sources are C, so the Jamfile uses `SubDirCcFlags`. A C++
consumer uses `SubDirC++Flags`.

The consumer of png applies the png package only:

```
if [ Packages-Includes png ]
{
    SubDirHdrs [ Packages-Includes png ] ;
}

if [ Packages-Defines png ]
{
    SubDirC++Flags [ Packages-Defines png ] ;
}

if [ Packages-Target png ]
{
    LinkLibraries tgt : [ Packages-Target png ] ;
}

LinkLibs  tgt : [ Packages-LinkLibs png ] ;
LinkFlags tgt : [ FLibPath [ Packages-LibDir png ] ]
                [ Packages-LinkFlags png ] ;
```

Prebuilt packages have no Jamfile, so `Prebuilt-Use` walks
`prebuilts.<pkg>.DEPENDS` for `INCLUDES` and `LIBDIR`. That walk is not
used for builtin packages. The source package `Jamconfigure` copies
dependency usage itself.

## Selection {#selection}

Builtin is used in two cases only:

- Override: `--with-inline-xxx` is given and `Builtin-Use` returns the
  package name. This overrides prebuilt and system/custom.
- Fallback: nothing earlier (prebuilt or system/custom) succeeded and
  `Builtin-Use` returns the package name.

Presence of the root alone does not select the package. Without the
option and without a failed earlier step the package is not builtin.

When builtin is selected the package-specific `*-Configure` is not
called. The source package has already published the six variables.
