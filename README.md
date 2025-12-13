# Platinum C++ Framework

Platinum (Pt) is a comprehensive C++ framework, which allows developers to
write high-performance applications for many platforms with only one codebase.
It provides a large amount of features and is still very easy to use. It
intergrates well into existing toolkits and frameworks.

## Contents

* [Building](#building)
* [License](#license)

## Building

### Web Assembly with Emscripten

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

## License

The Platinum Framework is licensed under the terms of the GNU LGPL
with extra permissions for static linking and to incorporate inlined
code from the library. The intent in licensing it in this way is to
provide it for use in all projects, both open and proprietary.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

As a special exception, you may use this file as part of a free
software library without restriction. Specifically, if other files
instantiate templates or use macros or inline functions from this
file, or you compile this file and link it with other files to
produce an executable, this file does not by itself cause the
resulting executable to be covered by the GNU General Public
License. This exception does not however invalidate any other
reasons why the executable file might be covered by the GNU Library
General Public License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301 USA

