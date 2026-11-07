# Jam Packages

## Documents

- [Package Interface](jam-packages-interface.md)
- [Prebuilt](jam-packages-prebuilt.md)
- [Builtin Source](jam-packages-builtin.md)

## Open Tasks

- [variables](jam-packages-interface.md#variables): Introduce the common package variables and accessors in `packages.jam` (`Packages-Includes` / `Packages-SetIncludes` and the rest, plus `Packages-Echo` and `Packages-Configure`).
- [modules](jam-packages-interface.md#modules): Split into `packages.jam` (interface only), `prebuilt.jam` and `builtin.jam`.
- [prebuilt-use](jam-packages-prebuilt.md#use): `Prebuilt-Use` fills the common flag variables via the accessors and returns the package name when the bundle contains it.
- [source-init](jam-packages-builtin.md#init): `Builtin-Use` registers the source root and returns the package name when present.
- [source-target](jam-packages-builtin.md#target): Source packages set `pkg.TARGET` (and usually `pkg.INCLUDES`). Consumers detect builtin by a non-empty target.
- [configure](jam-packages-interface.md#configure): In `Jamconfigure`, resolve each package with independent steps: builtin override (`--with-inline-xxx`), then prebuilt or system/custom via `*-Configure`, then builtin fallback. `Packages-Configure` only when found.

## Priority

Interface and modules first, then prebuilt and builtin Use rules, then Jamconfigure resolution.
