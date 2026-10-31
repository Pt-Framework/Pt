---
proposal: jam-packages.md
---

# Prebuilt Packages

A prebuilt bundle supplies compiled headers and libraries for one
platform. Its `jam/packages.jam` remains a pure data file of
`PREBUILT_PACKAGE_*` assignments. `prebuilt.jam` translates those
assignments into the common package variables.

## Baseline

`Prebuilt-Use` sets `PREBUILT_ROOT` and includes the manifest. The
rules `Prebuilt-Includes`, `Prebuilt-LinkLibs`, `Prebuilt-LibPath` and
`Prebuilt-LinkFlags` return the values that `Jamconfigure` currently
assigns with `?=` into `ZLIB_INCLUDES` and the other five variables
before it calls `ZLib-Configure`. The same pattern is used for png,
jpeg, FreeType and OpenSSL.

## Mapping {#mapping}

`prebuilt.jam` provides one rule that fills the common variables for a
package:

```
Prebuilt-Fill pkg
```

sets

- `pkg.INCLUDES` from `Prebuilt-Includes`
- `pkg.DEFINES` from the `PREBUILT_PACKAGE_DEFINES` list (via
  `FDefines`)
- `pkg.LINKLIBS` from `Prebuilt-LinkLibs`
- `pkg.LIBDIR` from `Prebuilt-LibPath`
- `pkg.LINKFLAGS` from `Prebuilt-LinkFlags`
- leaves `pkg.TARGET` empty

The existing `Prebuilt-*` rules stay. They continue to walk
`PREBUILT_PACKAGE_DEPENDS` for include paths (always) and for link
libraries (static linkage only).

The bundle manifest format is unchanged. `PREBUILT_SCHEMA` remains 1.
