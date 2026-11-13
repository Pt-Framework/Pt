\page jam-prebuilt-page Prebuilt Dependencies

A Jam project can take third-party libraries from a prebuilt dependency bundle
instead of from the system or from sources in the tree. A bundle is a directory
of already compiled headers and libraries for one platform. The published
bundles might include zlib, libpng, libjpeg, FreeType or OpenSSL.

The bundle is placed under the @c prebuilt directory in the project root. Each
platform has its own subdirectory, for example @c prebuilt/windows-x64-msvc-md.
Those files contain no absolute paths, so the directory can be moved or unpacked
as a whole.

Prebuilt libraries are used only when passing @c --with-prebuilt. They do not
replace the compiler, and they do not change the global compiler or linker
flags. See @ref jam-usage for the rest of the configure and build process.

@section jam-prebuilt-page-Using Using Prebuilt Dependencies

The usual command checks the matching bundle on the server, downloads it when
the manifest changed, then configures the build:

@code
jam.bat configure --with-prebuilt
./jam.sh configure --with-prebuilt
@endcode

Jam picks the platform folder from the current OS, CPU and toolset. An explicit
name pins the folder under @c prebuilt:

@code
./jam.sh configure --with-prebuilt=windows-x64-msvc-md
./jam.sh configure --with-prebuilt=macos-arm64-clang
@endcode

@c --with-prebuilt always contacts the network. A dropped connection fails
configure even when a cached folder is present. To use the tree on disk without
checking the server, add @c --prebuilt-local. That flag does nothing on its
own; it only modifies @c --with-prebuilt. A local-only name that is not on the
server uses the same pair. The command fails if the folder is missing.

@code
./jam.sh configure --with-prebuilt --prebuilt-local
./jam.sh configure --with-prebuilt=my-vendor --prebuilt-local
@endcode

The platform folder name is @c os-arch-toolset, with an extra suffix on
Windows. Jam builds it from @c TARGET_OS, @c TARGET_OSPLAT and @c TOOLSET:

- @c nt / @c win32 become @c windows; @c macosx becomes @c macos
- @c x86_64 / @c amd64 become @c x64; @c aarch64 becomes @c arm64
- Visual Studio toolsets @c vc* become @c msvc

Examples:

- @c windows-x64-msvc-md
- @c windows-x64-msvc-mdd
- @c macos-arm64-clang
- @c linux-x64-clang

Windows always has a suffix for the C runtime. A debug build without
optimization maps to @c mdd, otherwise @c md. Other operating systems omit the
suffix until a second runtime variant exists. The project's debug/release
setting and that Windows suffix are separate: on Linux and macOS the same
folder is used for both debug and release builds.

The default location is @c prebuilt/<name> in the project root. The download
uses the release tag @c PREBUILT_RELEASE (default @c prebuilt-1). The zip URL
is:

@verbatim
https://github.com/Pt-Framework/Pt/releases/download/<PREBUILT_RELEASE>/<name>.zip
@endverbatim

Settings made explicitly always win. If any of the dotted package flags
(@c pkg.INCLUDES, @c pkg.DEFINES, @c pkg.LINKLIBS, @c pkg.LIBDIR,
@c pkg.LINKFLAGS) are set on the command line or in the environment, or if
a flag is passed that builds that package from sources in the tree, prebuilt
paths are not applied to that package. The following commands keep a
hand-chosen zlib or build zlib from sources, while still using the bundle
for the other libraries:

@code
./jam.sh configure --with-prebuilt -szlib.INCLUDES=/opt/zlib/include
./jam.sh configure --with-prebuilt --with-inline-zlib
@endcode

Flags such as @c --with-inline-zlib, @c --with-inline-libpng,
@c --with-inline-libjpeg and @c --with-inline-freetype skip the corresponding
Configure probe and build that package from sources in the tree.

For zlib, png, jpeg and FreeType, configure looks for each library in this
order:

@verbatim
--with-inline-xxx  ->  Builtin-Configure
        |
        |  else
        v
Prebuilt-Use (only with --with-prebuilt)  ->  *-Configure pkg
        |
        |  else
        v
Builtin-Configure fallback
@endverbatim

@c Prebuilt-Use fills only still-empty flag variables. @c *-Configure pkg
then reads those values: prebuilt paths, @c -s hints, or an empty system
probe (pkg-config and compiler paths). There is no fallback from one
platform folder to another. If the chosen folder is missing, configure
reports an error. If the probe fails, zlib, png, jpeg and FreeType fall
back to sources in the tree.

OpenSSL and mbedTLS are the Pt-Ssl backends. Builtin runs only for
@c --with-inline-openssl or @c --with-inline-mbedtls. @c --with-openssl and
@c --with-mbedtls name the prebuilt or system backend. With no SSL flag,
configure probes openssl, then mbedtls, then @c nossl.

@section jam-prebuilt-page-Layout Bundle Directory Layout

One platform is one directory and one zip. Each library lives in its own
subdirectory, named after @c prebuilt.PACKAGES (@c zlib, @c png, @c jpeg,
@c freetype, @c openssl).

@verbatim
prebuilt/<name>/
  prebuilt.jam
  zlib/
    include/zlib.h
    lib/zlib.lib          (or libz.a / libz.dylib)
    bin/zlib.dll          (omitted for static)
  png/
    include/png.h
    lib/png16.lib
    bin/libpng16.dll
  jpeg/
    include/...
    lib/...
    bin/...
  freetype/
    include/ft2build.h
    include/freetype2/...
    lib/freetype.lib
    bin/freetype.dll
  openssl/
    include/openssl/...
    lib/libssl.lib libcrypto.lib
    bin/...
@endverbatim

Default locations, relative to @c prebuilt.ROOT:

- Headers: @c $(pkg)/include
- Libraries: @c $(pkg)/lib
- Binaries: @c $(pkg)/bin

@c prebuilt.<pkg>.INCLUDES is set only when that layout is not enough
(FreeType's @c include/freetype2). Those paths are added to the default
@c $(pkg)/include. The native @c lib/ and @c bin/ directories are kept
complete. There are no version directories and no @c debug/ folder inside
an @c md tree. Library names stay the same for a given platform folder
(@c zlib.lib in both @c md and @c mdd).

The directory can be moved as a whole because it contains no absolute paths.
The project calls @c Prebuilt-Load root once; that rule includes
@c prebuilt.jam from the bundle directory.

@section jam-prebuilt-page-Manifest The prebuilt.jam Manifest

Each bundle contains one Jam data file at @c prebuilt.jam in the bundle
root. It assigns variables only: no rules, no include guard, and no toolset
flags. The rules that use those variables live in @c jam/prebuilt.jam.
Unknown fields are ignored by Jam.

Bundle-wide variables:

- @c prebuilt.VERSION is the file format version. The project requires @c 1.
- @c prebuilt.ID is the same string as the directory and zip name.
- @c prebuilt.PACKAGES lists the package names that have dotted keys.

Per-package variables use the form @c prebuilt.<pkg>.<FIELD>. Omit empty
lists. Values are names, tokens, or paths relative to @c prebuilt.ROOT,
never raw flags (@c -l, @c -I, @c -D, @c -framework, @c /LIBPATH):

- @c prebuilt.<pkg>.LIBS: library names inside the bundle
  (@c zlib, @c png16, @c ssl @c crypto). Not @c -lssl or @c zlib.lib.
- @c prebuilt.<pkg>.DEPENDS: other packages in this bundle that
  this one needs.
- @c prebuilt.<pkg>.SYSLIBS: OS linker names that are not in the zip
  (@c crypt32, @c ws2_32). Jam applies @c FLinkLibs.
- @c prebuilt.<pkg>.FRAMEWORKS: macOS framework names.
- @c prebuilt.<pkg>.DEFINES: preprocessor tokens, not @c -D.
- @c prebuilt.<pkg>.INCLUDES: include paths relative to
  @c prebuilt.ROOT, only if not @c $(pkg)/include.
- @c prebuilt.<pkg>.LIBDIR: library paths relative to
  @c prebuilt.ROOT, only if not @c $(pkg)/lib.
- @c prebuilt.<pkg>.LINKFLAGS: extra linker flags.
- @c prebuilt.<pkg>.VERSION: version string for documentation.

Optional fields (ignored by Jam, available for other tools):

- @c prebuilt.<pkg>.LICENSE: SPDX identifier
- @c prebuilt.<pkg>.URL: upstream URL
- @c prebuilt.<pkg>.SHA256: checksum of key artifacts
- any other @c prebuilt.<pkg>.* key

Each package entry is the complete public usage of that package for the
linkage of this bundle: @c LIBS, @c SYSLIBS, @c FRAMEWORKS, @c DEFINES and
@c LINKFLAGS. The author copies public flags of dependencies onto the
consumed entry when a consumer of that entry needs them. A shared bundle
lists the public libraries. A static bundle lists the full closure in link
order.

@c DEPENDS is the dependency graph of the bundle. @c Prebuilt-Use walks it
only for include paths and library search directories. There is no linkage
field and no walk for @c LINKLIBS, @c DEFINES or @c LINKFLAGS. If png is in
the bundle, zlib is in the bundle too. There are no version ranges, no
optional dependencies, and no search for a package outside the bundle.

The file does not set @c pkg.INCLUDES. @c Prebuilt-Use fills the common
package variables from the manifest.

Example:

@code
# prebuilt.jam
prebuilt.VERSION  = 1 ;
prebuilt.ID       = windows-x64-msvc-md ;
prebuilt.PACKAGES = zlib png jpeg freetype openssl ;

prebuilt.zlib.LIBS     = zlib ;
prebuilt.zlib.VERSION  = 1.3.1 ;

prebuilt.png.LIBS      = png16 ;
prebuilt.png.DEPENDS   = zlib ;
prebuilt.png.VERSION   = 1.6.43 ;

prebuilt.jpeg.LIBS     = jpeg ;
prebuilt.jpeg.VERSION  = 3.0.3 ;

prebuilt.freetype.LIBS    = freetype ;
prebuilt.freetype.DEPENDS = png zlib ;
prebuilt.freetype.VERSION = 2.13.2 ;

prebuilt.openssl.LIBS     = ssl crypto ;
prebuilt.openssl.SYSLIBS  = crypt32 ws2_32 advapi32 user32 ;
prebuilt.openssl.VERSION  = 3.3.1 ;
@endcode

A macOS bundle uses a three-part @c prebuilt.ID such as @c macos-arm64-clang.
The Windows @c mdd folder uses @c prebuilt.ID = windows-x64-msvc-mdd and
different files on disk.

@section jam-prebuilt-page-Configure Configure Integration

Every package Configure rule takes the package name:

@verbatim
*-Configure pkg
@endverbatim

It reads the current @c pkg.* flag values as hints. If any of those is set,
pkg-config is skipped and the first link attempt uses the stored libraries.
If all five are empty, pkg-config and name guessing run. On success the
rule writes the five flags unmerged through @c Package-Set* (@c LIBDIR
still raw, @c LINKFLAGS still extra flags), persists them with
@c Package-Configure, and returns the package name. It does not set
@c pkg.TARGET.

Jamconfigure calls @c Prebuilt-Load root once after fetch. That binds the
bundle. @c Prebuilt-Use pkg fills still-empty @c pkg.* flag variables when
the bundle lists that package. It leaves @c pkg.TARGET empty.

zlib, png, jpeg and FreeType use independent @c ifs. Each step runs only
while @c WITH_pkg is empty:

@code
Load zlib.jam ;

if ! $(WITH_ZLIB) && ! ( --with-inline-zlib in $(ARGV) )
{
    if $(PT_WITH_PREBUILT)
    {
        Prebuilt-Use zlib ;
    }

    WITH_ZLIB = [ ZLib-Configure zlib ] ;
}

if ! $(WITH_ZLIB)
{
    WITH_ZLIB = [ Builtin-Configure zlib : $(PT_TOP) src zlib ] ;
}

Package-Echo zlib ;
@endcode

@c Package-Echo always prints whether the package was found as @c yes or @c no, then the six variables. @c *-Configure and
@c Builtin-Configure persist through @c Package-Configure when the package
was found. A builtin package @c Jamconfigure publishes the six variables,
including @c pkg.TARGET. There is no separate @c WITH_BUILTIN_xxx flag.

OpenSSL is package @c openssl, mbedTLS is package @c mbedtls. Jamconfigure
selects the Pt-Ssl backend with sequential @c if ! $(WITH_SSL) steps:
@c --with-inline-openssl, @c --with-openssl, @c --with-inline-mbedtls,
@c --with-mbedtls, then default openssl, then default mbedtls, then
@c nossl. Success sets @c WITH_SSL and @c PT_SSL_IMPL.

Jamfiles do not mention @c prebuilt.ROOT. They read the accessors:

@code
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
    LinkLibraries mylib : [ Package-Target png ] ;
}

LinkLibs  mylib : [ Package-LinkLibs png ] ;
LinkFlags mylib : [ FLibPath [ Package-LibDir png ] ]
                  [ Package-LinkFlags png ] ;
@endcode

@c pkg.LIBDIR is raw. The consumer applies @c FLibPath when passing it to
@c LinkFlags.

Include directories always cover dependent packages as well. @c png.h includes
@c zlib.h, and a Jamfile can call @c SubDirHdrs [ Package-Includes png ]
without adding zlib. @c Prebuilt-Use walks @c prebuilt.<pkg>.DEPENDS for
include directories and library search directories.
@c Prebuilt-Includes freetype returns FreeType's include dirs plus png and
zlib. @c Prebuilt-LibDir walks the same graph.

Link lines take @c LIBS, @c SYSLIBS and @c FRAMEWORKS from the named package
entry only. The entry is the complete public link line for this bundle.
MSVC OpenSSL still needs @c crypt32 @c ws2_32 @c advapi32 @c user32 on the
line (@c prebuilt.openssl.SYSLIBS).

@section jam-prebuilt-page-API Jam Rules

The rules live in @c jam/prebuilt.jam, loaded once per project. The bundle
file is @c prebuilt.jam. Toolset helpers stay in the project:
@c FLinkLibs, @c FIncludes, @c FDefines, @c FLibPath.

- @c Prebuilt-Id ( abi ? ) \n
  Builds the platform folder name from @c TARGET_OS, @c TARGET_OSPLAT and
  @c TOOLSET, with an optional extra suffix such as @c md.

- @c Prebuilt-Fetch ( url : root ) \n
  Downloads @c url when the remote @c url.manifest differs from
  @c root/manifest. Replaces @c root with the unzipped archive. Any download
  failure exits. The caller must run @c Fetch-Configure first.

- @c Prebuilt-Load ( root ) \n
  Binds the bundle at @c root, sets @c prebuilt.ROOT, includes @c prebuilt.jam,
  and exits if @c prebuilt.VERSION is not @c 1. The caller invokes this once
  when a prebuilt folder is selected.

- @c Prebuilt-Use ( pkg ) \n
  Returns @c pkg when the loaded bundle lists it, empty otherwise. On
  success fills still-empty @c pkg.INCLUDES, @c pkg.DEFINES,
  @c pkg.LINKLIBS, @c pkg.LIBDIR and @c pkg.LINKFLAGS. Leaves
  @c pkg.TARGET empty.

- @c Prebuilt-Has ( pkg ) \n
  Returns @c pkg when it is listed in @c prebuilt.PACKAGES, empty
  otherwise.

- @c Prebuilt-Depends ( packages + ) \n
  Walks @c prebuilt.<pkg>.DEPENDS without duplicates. Named packages first,
  dependencies last.

- @c Prebuilt-Includes ( packages + ) \n
  Include directories of those packages and the packages they depend on.
  Always @c $(prebuilt.ROOT)/$(pkg)/include, plus
  @c prebuilt.<pkg>.INCLUDES when set.

- @c Prebuilt-Defines ( packages + ) \n
  @c [ FDefines $(prebuilt.<pkg>.DEFINES) ] for the named packages only.

- @c Prebuilt-LinkLibs ( packages + ) \n
  @c prebuilt.<pkg>.LIBS, @c SYSLIBS and @c FRAMEWORKS of the named
  packages only. Always passed through @c FLinkLibs. Frameworks are
  appended as @c -framework name.

- @c Prebuilt-LibDir ( packages + ) \n
  Always @c $(prebuilt.ROOT)/$(pkg)/lib, plus @c prebuilt.<pkg>.LIBDIR
  when set, over the packages returned by @c Prebuilt-Depends.

- @c Prebuilt-LinkFlags ( packages + ) \n
  Extra flags from @c prebuilt.<pkg>.LINKFLAGS of the named packages
  only. Does not duplicate @c FLibPath; the caller adds that from
  @c LIBDIR.

- @c Prebuilt-Packages \n
  All package names exported by the bundle (@c prebuilt.PACKAGES).

- @c Prebuilt-Bin ( packages + ) / @c Prebuilt-Lib ( packages + ) \n
  All files below the native @c bin/ or @c lib/ directory of each named
  package. These rules do not walk dependencies.

@section jam-prebuilt-page-Deploy Deployment

Prebuilt bundles keep complete native @c bin/ and @c lib/ directories. They do
not say which files to install. A package @c bin/ can contain programs as well
as runtime libraries; Linux and macOS shared libraries commonly remain in
@c lib/.

The project chooses which packages to copy and whether to take @c bin/ or
@c lib/, then stages or installs with its own rules. For every package:

@code
local packages = [ Prebuilt-Packages ] ;
local sources = [ Prebuilt-Bin $(packages) ] ;
sources += [ Prebuilt-Lib $(packages) ] ;

for local src in $(sources)
{
    local dest = $(src:BS) ;
    File $(dest) : $(src) ;
    MakeLocate $(dest) : $(LOCATE_TARGET) ;
    Clean clean : $(dest) ;
    InstallFile $(BINDIR) : $(src) ;
}
@endcode

For a smaller set, pass package names to @c Prebuilt-Bin or @c Prebuilt-Lib.
To include files from packages that one of them depends on, first use
@c local packages = [ Prebuilt-Depends openssl ] ;.

@note Windows @c mdd is not redistributable. The debug CRT
(@c MSVCP*D.dll, @c VCRUNTIME*D.dll) is not in the Visual C++
redistributable. Release products use @c md. Local debug builds may place
@c mdd DLLs next to the executable.

@section jam-prebuilt-page-Produce Creating Prebuilt Dependencies

A bundle can be written by hand: fill the per-package directories, write
@c prebuilt.jam, and point @c --with-prebuilt --prebuilt-local at the
platform folder under @c prebuilt. Conan is not required. @c prebuilt.jam is
@c KEY = value assignments and is readable without running Jam.

A generator, including one driven by Conan in CI, writes the same layout:

- One directory per package: @c $(pkg)/include, @c $(pkg)/lib, @c $(pkg)/bin
- @c prebuilt.jam: library names to @c prebuilt.<pkg>.LIBS, other
  packages in this bundle to @c prebuilt.<pkg>.DEPENDS, OS linker names to
  @c prebuilt.<pkg>.SYSLIBS, macOS frameworks to
  @c prebuilt.<pkg>.FRAMEWORKS, extra include dirs to
  @c prebuilt.<pkg>.INCLUDES
- Complete @c lib/ and @c bin/ directories, without sorting or moving files
- Do not emit @c pkg.INCLUDES / @c pkg.LINKLIBS from the generator;
  @c Prebuilt-Use fills those from the layout
- Windows: two zips, @c windows-x64-msvc-md and @c windows-x64-msvc-mdd
- macOS / Linux: one folder name without a runtime suffix until a second
  variant exists
- Check: @c zlib/include/zlib.h, @c zlib/lib/zlib.lib or @c libz.a, @c bin/
  when shared, @c prebuilt.jam, no version directories, no absolute
  machine paths
