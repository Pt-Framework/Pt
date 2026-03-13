# Building

## Web Assembly with Emscripten

Possible build options include the following:

-sEMSDK               Path to Emscripten SDK (optional)
--with-sdl-raster     Enable SDL backend using software rendering (optional)

Example:
$ jam configure -sCONFIG=em-debug --optimize --debug -sTARGET\_OS=emscripten --with-sdl-raster -sEMSDK=path/to/emsdk

Run the program with emrun, after emsdk\_env.bat/sh. 


DISCLAIMER:

&nbsp; - SIDE Modules and MAIN MODULES have separate stacks, which are not

&nbsp;   properly synced. Therefore allocate all objects on the heap 

&nbsp; - works only with static linking: -sASSERTIONS=2 -sSAFE\_HEAP=1 -sSTACK\_OVERFLOW\_CHECK
