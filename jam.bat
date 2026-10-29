@echo off
setlocal

set "ROOT=%~dp0"
set "JAM=%ROOT%jam\core\bin\windows-x64\jam.exe"
set "SRC=%ROOT%jam\core\src"

if not exist "%JAM%" (
    echo Building jam.exe...

    where cl >nul 2>&1
    if errorlevel 1 (
        echo cl.exe not found. Run from a Visual Studio Developer Command Prompt.
        exit /b 1
    )

    pushd "%SRC%"
    cl /nologo /Fe..\jam.exe -DNT /MT /O2 -wd4996 ^
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
)

"%JAM%" %*
exit /b %ERRORLEVEL%
