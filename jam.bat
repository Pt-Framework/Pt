@echo off
setlocal EnableDelayedExpansion

set "JAM_PIN=4.0.0"
if not defined JAM_DIST_ROOT set "JAM_DIST_ROOT=https://github.com/Pt-Framework/Pt/releases/download"

set "ROOT=%~dp0"
if not defined JAM_ROOT set "JAM_ROOT=%ROOT%jam"

set "INCOMING=%JAM_ROOT%.incoming"
set "OUTGOING=%JAM_ROOT%.outgoing"
set "JAM_BINARY_DIR=%JAM_ROOT%\bin\windows-x64"
set "JAM=%JAM_BINARY_DIR%\jam.exe"
set "SRC=%JAM_ROOT%\src"

set "INIT="
if /i "%~1"=="init" (
    set "INIT=1"
    shift
)
if defined INIT goto parse_init

call :classify_snapshot "%JAM_ROOT%"
if /i "!SNAPSHOT_STATE!"=="invalid" goto invalid_snapshot
if /i not "!SNAPSHOT_STATE!"=="match" goto need_init
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
    if "!INIT_STATE!"=="--header" (
        set "INIT_HEADERS=!INIT_HEADERS! --header "!INIT_ARGUMENT!""
        for /f "delims=" %%H in ("!INIT_HEADERS!") do (
            endlocal
            set "INIT_HEADERS=%%H"
        )
    ) else (
        endlocal
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
if not exist "%JAM_ROOT%\" (
    if exist "%OUTGOING%\" move "%OUTGOING%" "%JAM_ROOT%" >nul
)
call :classify_snapshot "%JAM_ROOT%"
if /i "!SNAPSHOT_STATE!"=="match" goto build_host_binary
if /i "!SNAPSHOT_STATE!"=="invalid" goto invalid_snapshot
call :install_snapshot
if errorlevel 1 exit /b 1
goto build_host_binary

:build_host_binary
if exist "%JAM%" goto run_or_exit
if not exist "%SRC%\" goto invalid_snapshot
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

"%JAM%" -f "%JAM_ROOT%\Jambase" %*
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

set "ARCHIVE=%INCOMING%\jam.tar.gz"
set "ARCHIVE_URL=%JAM_DIST_ROOT%/jam-%JAM_PIN%/jam.tar.gz"

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

set "TAR=%SystemRoot%\System32\tar.exe"
if not exist "%TAR%" (
    where tar.exe >nul 2>&1
    if errorlevel 1 (
        echo Required tool "tar.exe" was not found.
        rd /s /q "%INCOMING%"
        exit /b 1
    )
    set "TAR=tar.exe"
)
"%TAR%" -xzf "%ARCHIVE%" -C "%INCOMING%"
if errorlevel 1 (
    echo Could not extract "%ARCHIVE%".
    rd /s /q "%INCOMING%"
    exit /b 1
)

set "STAGED=%INCOMING%\jam"
call :classify_snapshot "%STAGED%"
if /i not "!SNAPSHOT_STATE!"=="match" (
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

if exist "%JAM_ROOT%\" (
    if exist "%OUTGOING%\" rd /s /q "%OUTGOING%"
    move "%JAM_ROOT%" "%OUTGOING%"
    if errorlevel 1 (
        echo Could not move "%JAM_ROOT%" aside.
        rd /s /q "%INCOMING%"
        exit /b 1
    )
    move "%STAGED%" "%JAM_ROOT%"
    if errorlevel 1 (
        echo Could not install "%JAM_ROOT%".
        move "%OUTGOING%" "%JAM_ROOT%"
        rd /s /q "%INCOMING%"
        exit /b 1
    )
    rd /s /q "%OUTGOING%"
) else (
    move "%STAGED%" "%JAM_ROOT%"
    if errorlevel 1 (
        echo Could not install "%JAM_ROOT%".
        rd /s /q "%INCOMING%"
        exit /b 1
    )
)
rd /s /q "%INCOMING%"
exit /b 0

:classify_snapshot
set "CHECK_CORE=%~1"
set "SNAPSHOT_STATE="
if exist "%CHECK_CORE%\" goto classify_dir
if exist "%CHECK_CORE%" (
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)
set "SNAPSHOT_STATE=absent"
goto :eof

:classify_dir
if not exist "%CHECK_CORE%\meta\format" (
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)
if not exist "%CHECK_CORE%\meta\version" (
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)

set "FORMAT_LINE="
for /f "usebackq delims=" %%a in ("%CHECK_CORE%\meta\format") do (
    if defined FORMAT_LINE (
        set "SNAPSHOT_STATE=invalid"
        goto :eof
    )
    set "FORMAT_LINE=%%a"
)
if not "!FORMAT_LINE!"=="1" (
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)

set "VERSION_LINE="
for /f "usebackq delims=" %%a in ("%CHECK_CORE%\meta\version") do (
    if defined VERSION_LINE (
        set "SNAPSHOT_STATE=invalid"
        goto :eof
    )
    set "VERSION_LINE=%%a"
)
if not defined VERSION_LINE (
    set "SNAPSHOT_STATE=invalid"
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
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)
if not defined VERSION_MINOR (
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)
if not defined VERSION_PATCH (
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)
if defined VERSION_EXTRA (
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)
echo(!VERSION_MAJOR!| findstr /r /x "[1-9][0-9]*" >nul || (
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)
echo(!VERSION_MINOR!| findstr /r /x "[0-9][0-9]*" >nul || (
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)
echo(!VERSION_PATCH!| findstr /r /x "[0-9][0-9]*" >nul || (
    set "SNAPSHOT_STATE=invalid"
    goto :eof
)

if "!VERSION_LINE!"=="!JAM_PIN!" (
    set "SNAPSHOT_STATE=match"
) else (
    set "SNAPSHOT_STATE=mismatch"
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
echo Jam is missing or does not match snapshot %JAM_PIN% at "%JAM_ROOT%".
echo Run: "%~f0" init
exit /b 1

:invalid_snapshot
echo Jam is invalid at "%JAM_ROOT%".
echo Remove "%JAM_ROOT%" and run init to install snapshot %JAM_PIN%.
exit /b 1

:compiler_missing
echo The host binary "%JAM%" is missing.
echo Visual Studio 2017 or newer with the native C++ build tools is required.
echo Expected vswhere.exe at "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe".
exit /b 1

:filesystem_error
echo Could not create or access the host binary directory "%JAM_BINARY_DIR%".
exit /b 1
