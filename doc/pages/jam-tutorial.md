\page jam-tutorial Jam Tutorial

@section jam-tutorial-setup Adding Jam Support

This tutorial shows how to create a project with a self-contained Jam
build system. A build host needs only a native toolchain. Start with an
empty directory, from now on referred to as the project root, and place
sources, headers, and the files required for building with Jam there.

Copy the entry scripts and the Jam snapshot into the project root as
described in @ref jam-getting. After that step the project root contains
@c jam.sh, @c jam.bat, and @c jam/core/.

All examples in this tutorial use the script @c jam.sh when commands are
shown. On a Windows build host, run @c jam.bat instead of @c jam.sh,
with the exact same arguments. Unless noted otherwise, Jam is run from
the project root directory.

@section jam-tutorial-programs Building Programs

To build a simple hello world program, add two project files named
@c Jamconfigure and @c Jamfile. Configure writes @c Jamrules. After
adding those two files the project directory contains:

@verbatim
- jam.sh
- jam.bat
- [jam]
    - [core]
- Jamconfigure
- Jamfile
@endverbatim

Before a project can be built, it needs to be configured, so the build
host properties are determined, the toolset is detected, and so on. The
input for this configure step is the file named @c Jamconfigure. Add
the following lines to that file:

@code
SubDir MY_TOP ;

# configure basic C compiler
ConfigureCc ;

# configure basic C++ compiler
ConfigureC++ ;
@endcode

The @c SubDir rule defines the so-called root variable, which stands
for the project root directory. You can think of this for now simply as
the path to your project root directory. The @c SubDir rule also sets
the directory context for the following statements, until the next
@c SubDir call. It will be more useful when another project is included
as a subtree.
So far, only basic C and C++ support needs to be configured. This
allows programs, static libraries, and shared libraries to be built
using standard C and C++. The configure step can now be run with this
command:

@verbatim
./jam.sh configure
@endverbatim

The script @c jam.sh must have the executable bit set. If this is not
the case, add it by executing @c chmod +x jam.sh. On Windows, run
@c jam.bat instead of @c jam.sh.

After the build configuration, targets such as the hello world program
can be built. The build targets are defined in the file called
@c Jamfile in the project root, which is picked up by Jam during the
build step. Add the following lines to the Jamfile:

@code
SubDir MY_TOP ;
Main hello : main.cpp ;
@endcode

This Jamfile uses the @c Main rule to build a program named @c hello
from the source file @c main.cpp. The @c SubDir rule defines the root
variable and sets the directory context for the following statements,
just like it was done in the @c Jamconfigure file. Thus, @c main.cpp
will be looked for in the project root. If Jam is run now, a program
will be built named @c hello on Unix or @c hello.exe on Windows:

@verbatim
./jam.sh -q
@endverbatim

The command line parameter @c -q stops the build on the first error.
If it is left out, the build step continues and skips the targets that
fail to build.

@section jam-tutorial-clean Cleaning the Build

Jam can delete previously built targets from the project directory.
Execute Jam with the @c clean target:

@verbatim
./jam.sh clean
@endverbatim
