# Jam Initialization

## Documents

- [Distribution](jam-init-distribution.md)
- [Runtime](jam-init-runtime.md)

## Open Tasks

- [Distribution artifacts](jam-init-distribution.md#distribution-artifacts).
  Add `.github/workflows/jam-build.yml` to build the three host
  binaries, merge them into one snapshot archive, and publish a GitHub
  Release tagged `jam-<version>` with the archive, its SHA-256 sidecar,
  and the entry scripts.

- [Distribution initialization](jam-init-distribution.md#distribution-initialization).
  Split `meta/format` from `meta/version`, embed the snapshot pin and
  `JAM_DIST_ROOT` in the entry scripts, and have `--init` install
  `core/` when it is absent and replace it when the pin and
  `meta/version` differ.

## Priority

Distribution artifacts first, then distribution initialization.
