---
proposal: jam-packages.md
---

# Builtin Source Packages

A source package lives in a directory the parent names at configure.
It may be a plain directory or a git submodule. It is never fetched at
configure time. When selected it is built as part of the project and
exposes itself through the common package variables.

The package name is the public identity. Parent and package files use
the same name for `Builtin-Use`, `Package-Echo`, `Package-Configure`,
`Builtin-SubInclude`, `Builtin-Set`, `Builtin-Get`, and the six
usage variables. There is no renaming.

A builtin package is also a consumer of the package interface. It may
depend on other packages. Those dependencies are already resolved when
the builtin is configured. The builtin does not care whether a
dependency is itself builtin, prebuilt, or system. As a provider it
publishes complete public usage on the six variables, the same result
a consumer would get from pkgconf.

## Timing {#timing}

Package resolution in `Jamconfigure` finishes before any
`Builtin-SubInclude` runs. Dependencies are resolved before the
packages that need them. For the current set the order is zlib, then
png, jpeg, FreeType, then openssl.

When `Builtin-Use` includes a package `Jamconfigure`, and later when
the package `Jamfile` is entered, the six variables of every dependency
are already final. The `Jamconfigure` reads them through the accessors
to publish this package. The `Jamfile` reads them through the accessors
to compile.

## Init {#init}

`builtin.jam` is loaded by `Jamconfigure` and by the parent `Jamfile`.
Jambase does not load it. It provides:

```
Builtin-Top pkg
Builtin-SetTop pkg : value
Builtin-Use pkg : dir +
Builtin-Configure pkg
Builtin-SubInclude pkg
Builtin-Set pkg : var : value
Builtin-Get pkg : var
```

`Builtin-Use` joins `dir` with `FDirName` to form the source root and
returns the package name when the package succeeds, empty otherwise.

Presence is a directory check. There is no network access and no
download. When the root exists the rule sets `pkg.TOP`, includes the
package's `Jamconfigure`, and returns the package name when
`pkg.TARGET` is set. It does not persist `pkg.TOP`. The parent calls
`Builtin-Configure` when `Builtin-Use` succeeded.

- If the directory is missing the rule returns empty.
- If the package `Jamconfigure` is missing the rule calls `Exit`.
- If the package `Jamconfigure` calls `Exit` the whole configure aborts.
- If the package sets `pkg.TARGET` the rule returns the package name.

The parent names the source path once, at configure:

```
WITH_PNG = [ Builtin-Use png : $(PT_TOP) src libpng ] ;
if $(WITH_PNG)
{
    Builtin-Configure png ;
}

Package-Echo png ;
if $(WITH_PNG)
{
    Package-Configure png ;
}
```

## Target {#target}

The source package sets the common variables itself in its own
`Jamconfigure`, after `Builtin-Use` has set `pkg.TOP`:

```
Package-SetIncludes png : [ Builtin-Top png ] [ Package-Includes zlib ] ;
Package-SetTarget   png : png ;
```

(`z` or `libz`, whichever name the `Library` or `SharedLibrary` rule
uses.) `Package-SetTarget` sets this package's library name.

A non-empty `pkg.TARGET` is how `Builtin-Use` and consumers detect a
builtin package. There is no separate status flag and no separate
`WITH_BUILTIN_xxx`.

The package `Jamconfigure` uses `Builtin-Top` for its source root. It
does not mention the parent tree (`PT_TOP`) or the relative tokens the
parent passed to `Builtin-Use`.

## Descent {#descent}

The parent Jamfile includes a selected builtin with `Builtin-SubInclude`
only when `Package-Target` is set. The rule is not a no-op for an
unselected package.

`Builtin-SubInclude` sets the `SubDir` Jamrules gate `pkg.TOP-included`
and then `SubInclude pkg.TOP`. The package Jamfile calls `SubDir pkg.TOP`.
The gate stops `SubDir` from loading `$(pkg.TOP)/Jamrules`.

```
if [ Package-Target png ]
{
    Builtin-Set png : OUTPUT_TARGET : $(PT_OUTPUT_TARGET) ;
    Builtin-Set png : OUTPUT_OBJECT : $(PT_OUTPUT_OBJECT) ;
    Builtin-SubInclude png ;
}
```

The package Jamfile:

```
SubDir png.TOP ;
```

## Output properties {#output}

Locate is parent to builtin. The parent sets package-specific names
the Jamfile reads. The caller knows the names from that Jamfile. There
is no advertised need-list.

```
Builtin-Set pkg : var : value
Builtin-Get pkg : var
```

The values live as `pkg.var` (for example `png.OUTPUT_TARGET`). Names
are specific to the package. libpng uses `OUTPUT_TARGET` and
`OUTPUT_OBJECT`. Another package may use other names.

The package applies the real Jam rules when the property is set.
Unset means locate from `SubDir pkg.TOP`.

```
if [ Builtin-Get png : OUTPUT_OBJECT ]
{
    SubDirObjects [ Builtin-Get png : OUTPUT_OBJECT ] png ;
}

Library png : png.c ... ;

if [ Builtin-Get png : OUTPUT_TARGET ]
{
    MakeLocate png : [ Builtin-Get png : OUTPUT_TARGET ] ;
}
```

The parent sets the properties in the Jamfile before `Builtin-SubInclude`,
only when `Package-Target` is set. They are not persisted.

## Dependencies {#dependencies}

The source package `Jamconfigure` publishes complete public usage.
It sets this package's `INCLUDES` and `TARGET`, then copies each
dependency onto the flag variables. When a dependency is itself
builtin, it appends that dependency's `TARGET` so `LinkLibraries`
receives every Jam library the consumer needs. `Package-SetTarget`
still sets this package's own name.

The package `Jamfile` consumes dependencies through the accessors to
compile. A static `Library` does not link.

Example for png that depends on zlib:

```
# src/libpng/Jamconfigure
Package-SetIncludes png : [ Builtin-Top png ] [ Package-Includes zlib ] ;
Package-SetDefines  png : [ Package-Defines zlib ] ;
Package-SetTarget   png : png ;
Package-SetLinkLibs png : [ Package-LinkLibs zlib ] ;
Package-SetLibDir   png : [ Package-LibDir zlib ] ;
Package-SetLinkFlags png : [ Package-LinkFlags zlib ] ;

if [ Package-Target zlib ]
{
    png.TARGET += [ Package-Target zlib ] ;
}

# src/libpng/Jamfile
SubDir png.TOP ;

SubDirHdrs [ Package-Includes png ] ;
SubDirCcFlags [ Package-Defines png ] ;

if [ Builtin-Get png : OUTPUT_OBJECT ]
{
    SubDirObjects [ Builtin-Get png : OUTPUT_OBJECT ] png ;
}

Library png : png.c ... ;

if [ Builtin-Get png : OUTPUT_TARGET ]
{
    MakeLocate png : [ Builtin-Get png : OUTPUT_TARGET ] ;
}
```

The png sources are C, so the Jamfile uses `SubDirCcFlags`. A C++
consumer uses `SubDirC++Flags`.

The consumer of png applies the png package only:

```
if [ Package-Includes png ]
{
    SubDirHdrs [ Package-Includes png ] ;
}

if [ Package-Defines png ]
{
    SubDirC++Flags [ Package-Defines png ] ;
}

if [ Package-Target png ]
{
    LinkLibraries tgt : [ Package-Target png ] ;
}

LinkLibs  tgt : [ Package-LinkLibs png ] ;
LinkFlags tgt : [ FLibPath [ Package-LibDir png ] ]
                [ Package-LinkFlags png ] ;
```

Prebuilt packages have no Jamfile, so `Prebuilt-Use` walks
`prebuilt.<pkg>.DEPENDS` for `INCLUDES` and `LIBDIR`. That walk is not
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

The parent inverts the inline flag so `Builtin-Use` runs once. Prebuilt
and system run only when the flag is absent. `Builtin-Configure`
persists `pkg.TOP` when `Builtin-Use` succeeded:

```
if ! $(WITH_PNG) && ! ( --with-inline-libpng in $(ARGV) )
{
    Prebuilt-Use png ;

    WITH_PNG = [ Png-Configure [ Package-Includes png ]
                             : [ Package-Defines png ]
                             : [ Package-LinkLibs png ]
                             : [ Package-LibDir png ]
                             : [ Package-LinkFlags png ] ] ;
}

if ! $(WITH_PNG)
{
    WITH_PNG = [ Builtin-Use png : $(PT_TOP) src libpng ] ;
    if $(WITH_PNG)
    {
        Builtin-Configure png ;
    }
}
```
