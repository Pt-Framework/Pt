# Building

## Web Assembly with Emscripten

Possible build options include the following:

\-sEMSDK               Path to Emscripten SDK (optional)


Example:
$ jam configure -sCONFIG=em-debug --optimize --debug -sTARGET\_OS=emscripten -sEMSDK=path/to/emsdk

Run the program with emrun, after emsdk\_env.bat/sh.



DISCLAIMER:

  - SIDE Modules and MAIN MODULES have separate stacks, which are not

    properly synced. Therefore allocate all objects on the heap

  - works only with static linking: -sASSERTIONS=2 -sSAFE\_HEAP=1 -sSTACK\_OVERFLOW\_CHECK

