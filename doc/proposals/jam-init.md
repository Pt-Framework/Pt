# Jam Initialization

## Documents

- [Distribution](jam-init-distribution.md)
- [Runtime](jam-init-runtime.md)

## Open Tasks

- [Distribution initialization](jam-init-distribution.md#distribution-initialization).
  Split `meta/format` from `meta/version`, embed the snapshot pin and
  `JAM_DIST_ROOT` in the entry scripts, and have `--init` install
  `core/` when it is absent and replace it when the pin and
  `meta/version` differ.

## Priority

Distribution initialization.
