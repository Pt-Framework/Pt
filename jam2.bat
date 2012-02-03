@ECHO OFF

if not exist jam2.exe (
    echo "jam2.exe not found"
    goto :eof
)

call .\jam2.exe -fJambase1 %*
goto :eof

