\page jam-getting Getting Jam

Platinum versions a complete Jam snapshot in this repository. Other
projects copy the entry scripts and run @c init, which installs
@c core/ from the distribution.

@verbatim
- jam.sh
- jam.bat
- [jam]
    - [core]
        - Jambase
        - [meta]
        - [lib]
        - [src]
        - [bin]
    - [local]
@endverbatim

@c jam/core/ is the versioned runtime: the Jambase, Jam libraries,
sources for the host binary, packaged binaries, and snapshot metadata.
A @c jam/local/ directory is created only when the project adds local
extensions. That directory stays outside the versioned @c core/ tree.

@section jam-getting-scripts Entry Scripts

Two scripts start Jam. @c jam.sh runs on POSIX hosts. @c jam.bat runs
on Windows. They select the installed core Jambase and the host binary,
so the project working directory stays the current directory of the
invocation. @c Jamconfigure and @c Jamfile are therefore found in the
project.

When @c JAM_ROOT is unset, it defaults to @c jam below the directory
that contains the entry script. An external @c JAM_ROOT is any directory
the user sets. The scripts embed a snapshot pin and a default
@c JAM_DIST_ROOT of
@c https://github.com/Pt-Framework/Pt/releases/download. Setting
@c JAM_DIST_ROOT selects a mirror that serves the same
@c jam-4.0.0/jam-4.0.0.tar.gz layout.

The scripts select a host ID and start the host binary under
@c JAM_ROOT/core/bin, in the subdirectory named by that host ID,
passing @c -f with @c JAM_ROOT/core/Jambase. The initial host IDs are
@c windows-x64, @c linux-x64, and @c macos-universal. macOS always
selects @c macos-universal.

@section jam-getting-init Initialization

The entry scripts own the @c init command and consume it before they
invoke the Jam executable. @c init exits without running a project
build. It ignores arguments that follow the command and never creates,
removes, or otherwise modifies @c local/.

The pin is the snapshot version the scripts require. It matches
@c core/meta/version. A newer snapshot is taken by replacing both
scripts with the pair from that release, then running @c init.

When @c core/ is absent, or when it is valid and its version differs
from the pin, @c init downloads the pinned archive, checks the
format revision, the version, and the @c contents size inventory, and
installs it as @c core/. When @c core/ is present, valid, and already
at the pin, @c init only builds the selected host binary if that
binary is missing. When @c core/ is present but invalid, @c init
fails and leaves the tree. Remove @c core/ and run @c init to
install a new snapshot.

A normal invocation never downloads. It requires a @c core/ whose
version matches the pin and whose format revision is supported. If the
selected host binary is missing, the script may build that binary from
@c core/src/. If no compiler is available, it fails with an error that
identifies the missing host binary and the compiler requirement. If
@c core/ is absent, invalid, or names a different snapshot than the
pin, it fails with an instruction to run @c init.

On Windows a packaged @c windows-x64 binary is checked in under
@c jam/core/bin/windows-x64/. Other hosts build the host binary under
@c jam/core/bin/ from @c jam/core/src/ with @c cc on first use. A
published snapshot includes binaries for @c windows-x64, @c linux-x64,
and @c macos-universal.

@section jam-getting-load Loading Modules

@c Load is the rule that loads Jam runtime files. A relative name is
resolved first below @c core/lib/ under @c JAM_ROOT and, only if the
core file does not exist, below @c local/lib/ under @c JAM_ROOT. A
local file supplements the runtime and does not override a same-named
core file.

A project loads the extensions it needs from its @c Jamconfigure. For
example:

@code
Load company-tools.jam ;
@endcode

first tests:

@verbatim
<JAM_ROOT>/core/lib/company-tools.jam
<JAM_ROOT>/local/lib/company-tools.jam
@endverbatim

The first existing file is loaded. If no file exists, the error
identifies the requested name and both checked locations.
