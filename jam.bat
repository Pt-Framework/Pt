@echo off
setlocal EnableDelayedExpansion

set "JAM_PIN=4.0.0"
if not defined JAM_DIST_ROOT set "JAM_DIST_ROOT=https://github.com/Pt-Framework/Pt/releases/download"

set "ROOT=%~dp0"
if not defined JAM_ROOT set "JAM_ROOT=%ROOT%jam"

set "CORE=%JAM_ROOT%\core"
set "INCOMING=%JAM_ROOT%\core.incoming"
set "OUTGOING=%JAM_ROOT%\core.outgoing"
set "JAM_BINARY_DIR=%CORE%\bin\windows-x64"
set "JAM=%JAM_BINARY_DIR%\jam.exe"
set "SRC=%CORE%\src"

set "INIT="
if /i "%~1"=="init" (
    set "INIT=1"
    shift
)
if defined INIT goto parse_init

call :classify_core "%CORE%"
if /i "!CORE_STATE!"=="invalid" goto invalid_core
if /i not "!CORE_STATE!"=="match" goto need_init
goto build_host_binary

:parse_init
setlocal DisableDelayedExpansion
set "INIT_STATE="
set "INIT_HEADERS="

:parse_init_loop
if [%1]==[] goto init_parsed
if defined INIT_STATE (
    set "INIT_ARGUMENT=%~1"
    setlocal EnableDelayedExpansion
    set "INIT_HEADERS=!INIT_HEADERS! --header "!INIT_ARGUMENT!""
    for /f "delims=" %%H in ("!INIT_HEADERS!") do (
        endlocal
        set "INIT_HEADERS=%%H"
    )
    set "INIT_STATE="
) else (
    if "%~1"=="--header" set "INIT_STATE=--header"
)
shift
goto parse_init_loop

:init_parsed
setlocal EnableDelayedExpansion

:do_init
if not exist "%CORE%\" (
    if exist "%OUTGOING%\" move "%OUTGOING%" "%CORE%" >nul
)
call :classify_core "%CORE%"
if /i "!CORE_STATE!"=="match" goto build_host_binary
if /i "!CORE_STATE!"=="invalid" goto invalid_core
call :install_snapshot
if errorlevel 1 exit /b 1
goto build_host_binary

:build_host_binary
if exist "%JAM%" goto run_or_exit
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

:run_or_exit
if defined INIT exit /b 0

"%JAM%" -f "%CORE%\Jambase" %*
exit /b %ERRORLEVEL%

:install_snapshot
echo Installing Jam snapshot %JAM_PIN%...
if not exist "%JAM_ROOT%\" mkdir "%JAM_ROOT%"
if errorlevel 1 (
    echo Could not create "%JAM_ROOT%".
    exit /b 1
)

if exist "%INCOMING%\" rd /s /q "%INCOMING%"
mkdir "%INCOMING%"
if errorlevel 1 (
    echo Could not create "%INCOMING%".
    exit /b 1
)

set "ARCHIVE=%INCOMING%\jam-%JAM_PIN%.tar.gz"
set "ARCHIVE_URL=%JAM_DIST_ROOT%/jam-%JAM_PIN%/jam-%JAM_PIN%.tar.gz"

where curl.exe >nul 2>&1
if errorlevel 1 (
    echo Required tool "curl.exe" was not found.
    rd /s /q "%INCOMING%"
    exit /b 1
)
curl.exe -L -f -o "%ARCHIVE%" !INIT_HEADERS! "%ARCHIVE_URL%"
if errorlevel 1 (
    echo Could not download "%ARCHIVE_URL%".
    rd /s /q "%INCOMING%"
    exit /b 1
)

where tar.exe >nul 2>&1
if errorlevel 1 (
    echo Required tool "tar.exe" was not found.
    rd /s /q "%INCOMING%"
    exit /b 1
)
tar.exe -xzf "%ARCHIVE%" -C "%INCOMING%"
if errorlevel 1 (
    echo Could not extract "%ARCHIVE%".
    rd /s /q "%INCOMING%"
    exit /b 1
)

set "STAGED=%INCOMING%\core"
call :classify_core "%STAGED%"
if /i not "!CORE_STATE!"=="match" (
    echo Downloaded snapshot failed checks.
    rd /s /q "%INCOMING%"
    exit /b 1
)
call :check_contents "%STAGED%"
if errorlevel 1 (
    echo Downloaded snapshot failed checks.
    rd /s /q "%INCOMING%"
    exit /b 1
)

if exist "%CORE%\" (
    if exist "%OUTGOING%\" rd /s /q "%OUTGOING%"
    move "%CORE%" "%OUTGOING%"
    if errorlevel 1 (
        echo Could not move "%CORE%" aside.
        rd /s /q "%INCOMING%"
        exit /b 1
    )
    move "%STAGED%" "%CORE%"
    if errorlevel 1 (
        echo Could not install "%CORE%".
        move "%OUTGOING%" "%CORE%"
        rd /s /q "%INCOMING%"
        exit /b 1
    )
    rd /s /q "%OUTGOING%"
) else (
    move "%STAGED%" "%CORE%"
    if errorlevel 1 (
        echo Could not install "%CORE%".
        rd /s /q "%INCOMING%"
        exit /b 1
    )
)
rd /s /q "%INCOMING%"
exit /b 0

:classify_core
set "CHECK_CORE=%~1"
set "CORE_STATE="
if exist "%CHECK_CORE%\" goto classify_dir
if exist "%CHECK_CORE%" (
    set "CORE_STATE=invalid"
    goto :eof
)
set "CORE_STATE=absent"
goto :eof

:classify_dir
if not exist "%CHECK_CORE%\meta\format" (
    set "CORE_STATE=invalid"
    goto :eof
)
if not exist "%CHECK_CORE%\meta\version" (
    set "CORE_STATE=invalid"
    goto :eof
)

set "FORMAT_LINE="
for /f "usebackq delims=" %%a in ("%CHECK_CORE%\meta\format") do (
    if defined FORMAT_LINE (
        set "CORE_STATE=invalid"
        goto :eof
    )
    set "FORMAT_LINE=%%a"
)
if not "!FORMAT_LINE!"=="1" (
    set "CORE_STATE=invalid"
    goto :eof
)

set "VERSION_LINE="
for /f "usebackq delims=" %%a in ("%CHECK_CORE%\meta\version") do (
    if defined VERSION_LINE (
        set "CORE_STATE=invalid"
        goto :eof
    )
    set "VERSION_LINE=%%a"
)
if not defined VERSION_LINE (
    set "CORE_STATE=invalid"
    goto :eof
)

set "VERSION_MAJOR="
set "VERSION_MINOR="
set "VERSION_PATCH="
set "VERSION_EXTRA="
for /f "tokens=1-4 delims=." %%a in ("!VERSION_LINE!") do (
    set "VERSION_MAJOR=%%a"
    set "VERSION_MINOR=%%b"
    set "VERSION_PATCH=%%c"
    set "VERSION_EXTRA=%%d"
)
if not defined VERSION_MAJOR (
    set "CORE_STATE=invalid"
    goto :eof
)
if not defined VERSION_MINOR (
    set "CORE_STATE=invalid"
    goto :eof
)
if not defined VERSION_PATCH (
    set "CORE_STATE=invalid"
    goto :eof
)
if defined VERSION_EXTRA (
    set "CORE_STATE=invalid"
    goto :eof
)
echo(!VERSION_MAJOR!| findstr /r /x "[1-9][0-9]*" >nul || (
    set "CORE_STATE=invalid"
    goto :eof
)
echo(!VERSION_MINOR!| findstr /r /x "[0-9][0-9]*" >nul || (
    set "CORE_STATE=invalid"
    goto :eof
)
echo(!VERSION_PATCH!| findstr /r /x "[0-9][0-9]*" >nul || (
    set "CORE_STATE=invalid"
    goto :eof
)

if "!VERSION_LINE!"=="!JAM_PIN!" (
    set "CORE_STATE=match"
) else (
    set "CORE_STATE=mismatch"
)
goto :eof

:check_contents
set "CHECK_CORE=%~1"
if not exist "%CHECK_CORE%\meta\contents" (
    echo Snapshot contents inventory is missing.
    exit /b 1
)
for /f "usebackq tokens=1*" %%a in ("%CHECK_CORE%\meta\contents") do (
    set "SIZE=%%a"
    set "REL=%%b"
    if not defined SIZE (
        echo Snapshot contents inventory is invalid.
        exit /b 1
    )
    if not defined REL (
        echo Snapshot contents inventory is invalid.
        exit /b 1
    )
    echo(!SIZE!| findstr /r /x "[0-9][0-9]*" >nul || (
        echo Snapshot contents inventory is invalid.
        exit /b 1
    )
    set "REL_WIN=!REL:/=\!"
    set "FILE=%CHECK_CORE%\!REL_WIN!"
    if not exist "!FILE!" (
        echo Snapshot file "!REL!" is missing.
        exit /b 1
    )
    set "ACTUAL="
    for %%Z in ("!FILE!") do set "ACTUAL=%%~zZ"
    if not "!ACTUAL!"=="!SIZE!" (
        echo Snapshot file "!REL!" has size !ACTUAL!, expected !SIZE!.
        exit /b 1
    )
)
exit /b 0

:need_init
echo Jam core is missing or does not match snapshot %JAM_PIN% at "%JAM_ROOT%".
echo Run: "%~f0" init
exit /b 1

:invalid_core
echo Jam core is invalid at "%JAM_ROOT%".
echo Remove core/ and run init to install snapshot %JAM_PIN%.
exit /b 1

:compiler_missing
echo The host binary "%JAM%" is missing.
echo Visual Studio 2017 or newer with the native C++ build tools is required.
echo Expected vswhere.exe at "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe".
exit /b 1

:filesystem_error
echo Could not create or access the host binary directory "%JAM_BINARY_DIR%".
exit /b 1
