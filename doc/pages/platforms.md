\page platforms Supported Platforms

One codebase. The same public API on every target Jam can configure.

<h2>Windows</h2>
Desktop Windows with the Visual C++ toolset. MinGW is available.

<h2>Linux</h2>
Desktop and embedded Linux on common CPU families, using GCC or Clang.

<h2>macOS</h2>
macOS with the compilers from Xcode.

<h2>WebAssembly</h2>
Cross-compiled with Emscripten for browsers and wasm runtimes.
Set TARGET_OS to `emscripten` when configuring.

<h2>QNX</h2>
QNX Neutrino with the QCC compiler.

<h2>Unix</h2>
AIX and Solaris with the system compilers or GCC.
