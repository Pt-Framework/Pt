---
proposal: jam-init.md
---

# Jam Initialization Distribution

## Purpose

Jam is maintained as an independent project while remaining a vendored build
system for its consumers. The first implementation keeps a complete Jam
snapshot in the Pt repository. Its entry scripts prepare and start that local
snapshot without a distribution server.

The entry scripts are intentionally small shell and batch programs. Their
bootstrap model must remain practical to implement in POSIX shell, Windows
batch, and, where useful, PowerShell. The first implementation does not
require a separate configuration file, an offline-mode protocol, a download
protocol, or archive extraction.

## Installation root

`JAM_ROOT` identifies the root of one Jam installation. It contains both the
release-managed core and the user-owned local extensions:

```text
<JAM_ROOT>/
  core/
  local/
```

When `JAM_ROOT` is unset, it defaults to `jam` below the directory containing
the entry script. In the first implementation, an external `JAM_ROOT` must
already contain a prepared snapshot supplied by build-server infrastructure.

The Pt repository versions `jam/core/` as its vendored snapshot. Projects
that later initialize `jam/core/` from a distribution may ignore it, while
`jam/local/` remains visible and may be versioned with the project's build
configuration.

## Snapshot layout

`core/` is a complete, versioned Jam snapshot:

```text
<JAM_ROOT>/core/
  manifest
  runtime/
    Jambase
    *.jam
  src/
    *.c
    *.h
    modules/
  bin/
    <host-id>/
      jam[.exe]
```

The manifest identifies the pinned Jam version and source revision and records
the snapshot contents and available host binaries. Runtime scripts, sources,
and binaries in a snapshot represent the same Jam revision.

A snapshot may omit a binary for a host. It still contains the sources
necessary to build that binary. This makes the same snapshot both a consumer
installation and the input to a release worker that creates a new host binary
for later distribution.

The host-ID format and manifest encoding must let an entry script validate that
an installed core is complete and select a binary for the current host.

## Phase 1: vendored snapshot

The first implementation restructures the existing vendored Jam files into
`jam/core/`. Runtime scripts reside below `core/runtime/`, C sources reside
below `core/src/`, and generated host binaries reside below
`core/bin/<host-id>/`. `jam/local/` is created only when users add local
extensions.

The Pt checkout is the source of the snapshot. No download source, archive
format, or distribution environment variable exists in this phase.

## Phase 1: local initialization

The entry scripts own `--init` and remove it before invoking the Jam
executable. `--init` validates the vendored `core/` snapshot and builds a
binary for the current host when `core/bin/<host-id>/` lacks one. It exits
successfully after initialization and does not run a project build.

A normal invocation requires a complete installed `core/`. If `core/` is
present but its current-host binary is missing, the entry script may build it
from `core/src/`. If no compiler is available, it fails with an error that
identifies the missing host binary and compiler requirement. If `core/` is
absent, it fails with an instruction to run `--init` or provide a prepared
`JAM_ROOT`.

The bootstrap scripts must report validation, compiler, and filesystem
failures explicitly. They do not silently substitute a different installation
root or host binary.

`--update` is not a public command in this phase. Without a distribution
source, an update operation would either do nothing or misleadingly suggest
that it could obtain a newer snapshot.

The entry scripts own generated binaries below `core/bin/`; they do not
otherwise replace the vendored `core/` snapshot. They never create, remove,
replace, or otherwise modify `local/`.

## Phase 2: distribution artifacts

A release worker initializes the vendored source snapshot on each supported
host and publishes its generated binary together with the corresponding
runtime, sources, and manifest. This phase defines archive names, host IDs,
manifest contents, and integrity validation. It does not change the local
runtime model.

## Phase 3: distribution initialization

After published snapshots exist, `JAM_DIST_ROOT` identifies the HTTP(S) base
URL for pinned Jam distribution snapshots. The entry scripts use their
embedded default when it is unset; setting it selects a release mirror.

`--init` downloads and validates the pinned snapshot when `core/` is absent,
stages it in a temporary directory, and installs it atomically. If the
snapshot lacks a binary for the current host, it builds one from `core/src/`.

`--update` downloads and validates the pinned snapshot, stages it, and
atomically replaces `core/`. It does not select a different Jam version. If
the replacement lacks the current host binary, it builds that binary from the
replacement sources. It never changes `local/`.

An interrupted or failed initialization or update must not leave a partial
`core/` that appears valid. Download, validation, extraction, compiler, and
filesystem failures are reported explicitly. The scripts do not silently
substitute a different Jam version, remote, or installation root.

## Baseline

The current entry scripts use a vendored `jam/` directory in the project.
`jam.bat` starts `jam/jam.exe` and builds it with MSVC when it is absent.
`jam.sh` starts `jam/jam` and builds it with `cc` when it is absent. The
current layout has no versioned snapshot, no explicit initialization command,
and no separate ownership boundary for user extensions.
