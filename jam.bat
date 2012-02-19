@ECHO OFF

if not exist jam.exe (
    echo "jam.exe not found"

    cl.exe
    if %errorlevel% neq 0 (
        echo no cl.exe
    ) 

    goto :eof
)

call .\jam.exe %*
goto :eof

