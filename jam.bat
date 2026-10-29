@echo off
setlocal

set "ROOT=%~dp0"
if not defined JAM_ROOT set "JAM_ROOT=%ROOT%jam"

set "CORE=%JAM_ROOT%\core"
set "VERSION=%CORE%\meta\version"
set "JAM_BINARY_DIR=%CORE%\bin\windows-x64"
set "JAM=%JAM_BINARY_DIR%\jam.exe"
set "SRC=%CORE%\src"

set "INIT="
:parse_arguments
if "%~1"=="" goto arguments_parsed
if /i "%~1"=="--init" set "INIT=1"
shift
goto parse_arguments

:arguments_parsed
if not exist "%VERSION%" goto invalid_core

set "VERSION_MAJOR="
set "VERSION_MINOR="
set "VERSION_PATCH="
set "VERSION_FORMAT="
set "VERSION_EXTRA="
for /f "usebackq tokens=1-5" %%a in ("%VERSION%") do (
    if defined VERSION_FORMAT goto invalid_core
    set "VERSION_MAJOR=%%a"
    set "VERSION_MINOR=%%b"
    set "VERSION_PATCH=%%c"
    set "VERSION_FORMAT=%%d"
    set "VERSION_EXTRA=%%e"
)

if not defined VERSION_MAJOR goto invalid_core
if not defined VERSION_MINOR goto invalid_core
if not defined VERSION_PATCH goto invalid_core
if not defined VERSION_FORMAT goto invalid_core
if defined VERSION_EXTRA goto invalid_core
echo(%VERSION_MAJOR%| findstr /r /x "[1-9][0-9]*" >nul || goto invalid_core
echo(%VERSION_MINOR%| findstr /r /x "[0-9][0-9]*" >nul || goto invalid_core
echo(%VERSION_PATCH%| findstr /r /x "[0-9][0-9]*" >nul || goto invalid_core
if not "%VERSION_FORMAT%"=="1" goto invalid_core

if not exist "%JAM%" (
    if not exist "%SRC%\" goto invalid_core
    echo Building jam.exe...

    setlocal EnableDelayedExpansion
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if not exist "!VSWHERE!" goto compiler_missing

    set "VSINSTALLATION="
    for /f "usebackq delims=" %%a in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        if not defined VSINSTALLATION set "VSINSTALLATION=%%a"
    )
    if not defined VSINSTALLATION goto compiler_missing

    set "VSDEVCMD=!VSINSTALLATION!\Common7\Tools\VsDevCmd.bat"
    if not exist "!VSDEVCMD!" goto compiler_missing
    call "!VSDEVCMD!" -arch=x64 -host_arch=x64
    if errorlevel 1 goto compiler_missing

    where cl >nul 2>&1
    if errorlevel 1 (
        goto compiler_missing
    )

    if not exist "%JAM_BINARY_DIR%\" mkdir "%JAM_BINARY_DIR%"
    if errorlevel 1 goto filesystem_error

    pushd "%SRC%"
    if errorlevel 1 goto filesystem_error
    cl /nologo /Fe"%JAM%" -DNT /MT /O2 -wd4996 ^
        builtins.c class.c command.c compile.c constants.c debug.c debugger.c execcmd.c execnt.c ^
        filent.c filesys.c function.c frames.c glob.c hash.c hcache.c hdrmacro.c ^
        headers.c jam.c jambase.c jamgram.c lists.c make.c make1.c md5.c mem.c modules.c ^
        native.c object.c option.c output.c parse.c pathnt.c pathsys.c cwd.c regexp.c rules.c scan.c ^
        search.c strings.c subst.c timestamp.c variable.c w32_getreg.c ^
        modules\set.c modules\path.c modules\regex.c modules\property-set.c modules\sequence.c ^
        modules\order.c ^
        kernel32.lib advapi32.lib user32.lib
    if errorlevel 1 (
        popd
        exit /b 1
    )
    del /q *.obj 2>nul
    popd
    endlocal
)

if defined INIT exit /b 0

"%JAM%" -f "%CORE%\Jambase" %*
exit /b %ERRORLEVEL%

:invalid_core
echo Jam core is missing or invalid at "%JAM_ROOT%".
echo Provide a prepared JAM_ROOT with supported snapshot format revision 1.
exit /b 1

:compiler_missing
echo The host binary "%JAM%" is missing.
echo Visual Studio 2017 or newer with the native C++ build tools is required.
echo Expected vswhere.exe at "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe".
exit /b 1

:filesystem_error
echo Could not create or access the host binary directory "%JAM_BINARY_DIR%".
exit /b 1
