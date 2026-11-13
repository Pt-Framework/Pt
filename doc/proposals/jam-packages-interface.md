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

The six variables are the complete public usage of the package, the
same result a consumer would get from pkgconf. Consumers read only
these variables. Package-specific `*-Configure` rules stay ignorant of
whether the values came from a user, a prebuilt bundle, a source tree,
or a system probe.

Prebuilt and system packages fill the flag variables and leave
`TARGET` empty. A source package fills `TARGET` with its Jam library
target and usually `INCLUDES`. When it depends on other packages it
copies their public usage onto its own variables.

A non-empty `pkg.TARGET` means the package is builtin (source-built).
Prebuilt and system leave `TARGET` empty.

## Variables {#variables}

The common variables are dotted: `$(pkg.INCLUDES)`, `$(pkg.DEFINES)`,
`$(pkg.LINKLIBS)`, `$(pkg.LIBDIR)`, `$(pkg.LINKFLAGS)`, `$(pkg.TARGET)`.

`pkg.TARGET` is the Jam target name of a source-built library. Empty
when the package is prebuilt or system. Consumers call `LinkLibraries`
with that name when it is set. They also apply `LINKLIBS`, `FLibPath`
of `LIBDIR`, and `LINKFLAGS`.

`pkg.INCLUDES` is raw include directories. `pkg.DEFINES` is formatted
compiler flags (`-DFOO=1` / `/DFOO=1`), the result of `FDefines`.
The manifest stores raw tokens (`FOO=1`); `Prebuilt-Enable` converts them
with `FDefines` when filling `pkg.DEFINES`. `pkg.LINKLIBS` is
link-library flags. `pkg.LIBDIR` is raw library search directories.
`pkg.LINKFLAGS` is extra linker flags. Empty lists are omitted:
`SubDirHdrs` and `SubDirCcFlags` run only when the package supplied
includes or defines.

A consumer applies the variables with the ordinary Jam rules:

```
if [ Package-Includes pkg ]
{
    SubDirHdrs [ Package-Includes pkg ] ;
}

if [ Package-Defines pkg ]
{
    SubDirCcFlags [ Package-Defines pkg ] ;
}

if [ Package-Target pkg ]
{
    LinkLibraries tgt : [ Package-Target pkg ] ;
}

LinkLibs  tgt : [ Package-LinkLibs pkg ] ;
LinkFlags tgt : [ FLibPath [ Package-LibDir pkg ] ]
                [ Package-LinkFlags pkg ] ;
```

C targets use `SubDirCcFlags`. C++ targets use `SubDirC++Flags`.

Consumers, user overrides and Configure rules use only the dotted form.
The former five-tuple names (`ZLIB_*`, `PNG_*`, `JPEG_*`, `FREETYPE_*`,
`SSL_*`) are not produced and are not read.

`package.jam` provides accessors so callers do not spell the dotted
names by hand:

```
Package-Includes pkg
Package-Defines pkg
Package-LinkLibs pkg
Package-LibDir pkg
Package-LinkFlags pkg
Package-Target pkg

Package-SetIncludes pkg : values *
Package-SetDefines pkg : values *
Package-SetLinkLibs pkg : values *
Package-SetLibDir pkg : values *
Package-SetLinkFlags pkg : values *
Package-SetTarget pkg : value
```

Getters return the current list (empty if unset). Setters replace the
list. Direct assignment to the dotted variables remains valid; the
accessors are the preferred form.

`package.jam` also provides:

```
Package-Status pkg
Package-Configure pkg
```

`Package-Status` prints whether the package was found, then the six
variables via `EchoOption`, including empty lists. The found line is
`yes` after `Package-Configure`, otherwise `no`. The bracket names the
package, not `pkg.FOUND`. It is the detail primitive. The configure
run calls it after each package chain.
`Package-Configure` persists the six names via `Configure` and sets
`pkg.FOUND` to `yes`. It does not persist `pkg.FOUND`. `pkg.FOUND` is
configure reporting. It is not one of the six usage variables, and
setting it does not select the package.

## Modules {#modules}

Three modules own the mechanism:

- `package.jam` defines the common variable interface, its accessors,
  `Package-Status` and `Package-Configure`. It knows nothing about
  prebuilt bundles, source package roots, configure options, or
  resolution order.
- `prebuilt.jam` maps a prebuilt bundle onto the common variables.
- `builtin.jam` selects source packages from a root directory.

`package.jam` is included from Jambase. `prebuilt.jam` is loaded on
demand by `Jamconfigure`. `builtin.jam` is loaded by `Jamconfigure`
and by the parent `Jamfile`.

## Configure {#configure}

Resolution order and Configure dispatch live in `Jamconfigure`. It is
the only place that knows about user settings, configure options,
prebuilt and builtin.

For each package the steps are independent `if`s. Each step runs only
while `WITH_pkg` is still empty. The first success sets `WITH_pkg` to
the package name the winning rule returns. Empty means not found.
`WITH_pkg` is a configure selection. It is not persisted and it is not
how a consumer detects the package.

1. Builtin override: `--with-inline-xxx` is given and
   `Builtin-Configure pkg : dir` succeeds. This overrides every other
   source.
2. Prebuilt or system/custom: when `--with-prebuilt` is selected,
   `Jamconfigure` calls `Prebuilt-Import root` once after fetch.
   `Prebuilt-Enable` runs only when `PT_WITH_PREBUILT` is set. It fills
   still-empty flag variables when the bundle contains the package
   (side effect only). Already set values (including `-s` overrides)
   are left unchanged. Then the package-specific `*-Configure pkg`
   reads the current package state (prebuilt paths, `-s` hints, or
   empty for a system probe). Custom `-s` settings are hints for the
   system probe. They sit in `pkg.*` before any rule runs.
3. Builtin fallback: `Builtin-Configure pkg : dir` succeeds when
   nothing earlier did.

`Prebuilt-Enable` returns the package name when the bundle contains it,
empty otherwise. Its return value is not assigned to `WITH_pkg`. It
only fills still-empty flag variables.

`Builtin-Configure pkg : dir` joins `dir` to the source root, sets
`pkg.TOP`, and includes the source package's `Jamconfigure`. That file
publishes the six variables only after its checks succeed. A non-empty
`pkg.TARGET` is the success signal. On success the rule persists
`pkg.TOP` and the six common variables via `Package-Configure` and
returns the package name. A missing directory returns empty. A missing
package `Jamconfigure` is an `Exit`. A configure error in the package
`Jamconfigure` calls `Exit` and aborts. The rule does not set
`pkg.TARGET`.

A `*-Configure` rule takes the package name. It reads the current flag
values from the accessors as hints. It does not know whether they came
from a prebuilt bundle, a user override, or an empty system probe.

It probes with `CheckBuildC++`. `FIncludes` of `INCLUDES` and `FLibPath`
of `LIBDIR` appear only on the probe line, together with `LINKLIBS` and
`LINKFLAGS`. On success it writes the five flag variables unmerged
through the `Package-Set*` accessors (`LIBDIR` still raw, `LINKFLAGS`
still extra flags), persists them with `Package-Configure`, and returns
the package name. On failure it returns empty and leaves the variables
unchanged. It does not set `pkg.TARGET`.

`pkg.TARGET` is set only by a builtin package `Jamconfigure`. Consumers
detect a source package by a non-empty target. `WITH_BUILTIN_xxx` holds
the inline option during configure. It is not persisted and it is not
how a consumer detects a builtin package.

`Package-Configure` is the persist primitive for the six names.
`*-Configure` and `Builtin-Configure` call it on success. That call
also sets `pkg.FOUND` to `yes`. An unfound package does not call it,
so `pkg.FOUND` stays empty and no values are persisted for later build
steps.

When builtin is selected the package-specific `*-Configure` is not
called. The source package `Jamconfigure` has already published the
six variables.

## Reporting {#reporting}

`Jamconfigure` calls `Package-Status` after the chain. That is the whole
package report. `Package-Status` prints the found line first, then the
six variables. The found value is `yes` when `Package-Configure` ran,
and `no` when it did not. The bracket names the package, not
`pkg.FOUND`. The line does not name the origin. The six
brackets name the usage variables, so the user sees what to override
with `-s`. `pkg.LINKLIBS` is one of those lines. A set `pkg.TARGET` is
a builtin package. A set `pkg.LIBDIR` with an empty `pkg.TARGET` is a
prebuilt package. A system find has neither.

A failed stage is silent. Probe attempts (`ZLib-Probe`,
`PkgConfig-Exists`, individual link-name guesses) print nothing. Their
detail is already in `ConfigureLog`. `*-Configure` and
`Builtin-Configure` do not print and do not call `Package-Status`.
`Prebuilt-Enable` prints nothing. It fills hints.

`Jamconfigure` drops the found-line. The selection lines stay. Those
are `--with-inline-*`, and for SSL `WITH_SSL` and `PT_SSL_IMPL`.

```
Package-Status zlib ;
```

```
- zlib found [zlib]: yes
- zlib includes [zlib.INCLUDES]: <pkg.INCLUDES>
- zlib defines [zlib.DEFINES]: <pkg.DEFINES>
- zlib libs [zlib.LINKLIBS]: <pkg.LINKLIBS>
- zlib library dir [zlib.LIBDIR]: <pkg.LIBDIR>
- zlib link flags [zlib.LINKFLAGS]: <pkg.LINKFLAGS>
- zlib target [zlib.TARGET]: <pkg.TARGET>
```

When nothing was found, `pkg.FOUND` prints `no` and the six variables
are empty. The same reporting applies to png, jpeg and FreeType. For
SSL, `Package-Status` follows for the selected backend and prints that
package's found flag. `nossl` has no package to print.

### zlib example

```
Load zlib.jam ;

WITH_BUILTIN_ZLIB = [ HasOption --with-inline-zlib ] ;
EchoOption --with-inline-zlib : "Using inline zlib" : $(WITH_BUILTIN_ZLIB) ;

if ! $(WITH_BUILTIN_ZLIB)
{
    if $(PT_WITH_PREBUILT)
    {
        Prebuilt-Enable zlib ;
    }

    WITH_ZLIB = [ ZLib-Configure zlib ] ;
}

if ! $(WITH_ZLIB)
{
    WITH_ZLIB = [ Builtin-Configure zlib : $(PT_TOP) src zlib ] ;
}

Package-Status zlib ;
```

The same pattern applies to png, jpeg and FreeType. `--with-inline-xxx`
skips prebuilt and system. `Builtin-Configure` runs once as override or
as fallback.

### SSL backends

`Jamconfigure` selects the Pt-Ssl backend from packages `openssl` and
`mbedtls`. Builtin only with `--with-inline-openssl` or
`--with-inline-mbedtls`. `--with-openssl` and `--with-mbedtls` name the
prebuilt or system backend.

Independent `if`s, each `if ! $(WITH_SSL)`. Explicit flags run first:

1. `--with-inline-openssl`: `Builtin-Configure openssl : dir`
2. `--with-openssl`: `Prebuilt-Enable openssl` and `OpenSsl-Configure openssl`
3. `--with-inline-mbedtls`: `Builtin-Configure mbedtls : dir`
4. `--with-mbedtls`: `Prebuilt-Enable mbedtls` and `MbedTls-Configure mbedtls`
5. Default openssl: `Prebuilt-Enable openssl` and `OpenSsl-Configure openssl`
6. Default mbedtls: `Prebuilt-Enable mbedtls` and `MbedTls-Configure mbedtls`
7. `PT_SSL_IMPL = nossl`

Success sets `WITH_SSL` and `PT_SSL_IMPL` to `openssl` or `mbedtls`.
`Pt-Ssl` reads the accessors of that package. The bcrypt probe for
builtin mbedtls lives in `src/mbedtls/Jamconfigure`.
