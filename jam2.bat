@ECHO OFF

if not exist jam2.exe (
    echo "jam2.exe not found"

    cl.exe
    if %errorlevel% neq 0 (
        echo no cl.exe
    ) 

    goto :eof
)

call .\jam2.exe -fJambase1 %*
goto :eof

