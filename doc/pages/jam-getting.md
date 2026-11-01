\page jam-getting Getting Jam

The Platinum repository contains a complete Jam snapshot. A project
includes that snapshot with its sources so a build host needs only a
native toolchain. Copy the entry scripts and the @c jam/core/ directory
into the project root as provided:

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
that contains the entry script. An external @c JAM_ROOT must already
contain a prepared snapshot.

The scripts select a host ID and start the host binary under
@c JAM_ROOT/core/bin, in the subdirectory named by that host ID,
passing @c -f with @c JAM_ROOT/core/Jambase. The initial host IDs are
@c windows-x64, @c linux-x64, and @c macos-universal. macOS always
selects @c macos-universal.

@section jam-getting-init Initialization

The entry scripts own @c --init and remove it before they invoke the
Jam executable. @c --init validates the supported snapshot format
revision in @c core/ and builds a binary for the selected host when
that binary is missing. It then exits without running a project build.

A normal invocation also requires a @c core/ with a supported format
revision. If @c core/ is present and the selected host binary is
missing, the script may build that binary from @c core/src/. If no
compiler is available, it fails with an error that identifies the
missing host binary and the compiler requirement. If @c core/ is
absent or invalid, it fails with an instruction to provide a prepared
@c JAM_ROOT.

On Windows a packaged @c windows-x64 binary is checked in under
@c jam/core/bin/windows-x64/. Other hosts build the host binary under
@c jam/core/bin/ from @c jam/core/src/ with @c cc on first use.

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
