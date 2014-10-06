set SolutionDir=%~dp0
set Config=%1
set ProjectName=%2
set Top=%SolutionDir%..\..\

cd %Top%

if "%Config%" == "nt-vc10-Win32-Debug" (
call jam.bat configure --debug -sCONFIG=%Config% -sTOOLSET=vc10 
)

if "%Config%" == "nt-vc10-Win32-Release" (
call jam.bat configure --optimize -sCONFIG=%Config% -sTOOLSET=vc10 
)

if "%Config%" == "linux-gcc-arm-Debug" (
call jam.bat configure --debug -sCONFIG=%Config% -sTARGET_OS=linux -sTARGET_OSPLAT=arm -sTOOLSET_ROOT="%LINARO_ARM_GNUABIHF_2013_11_R1%\bin\arm-linux-gnueabihf-" 
)

if "%Config%" == "linux-gcc-arm-Release" (
call jam.bat configure --optimize -sCONFIG=%Config% -sTARGET_OS=linux -sTARGET_OSPLAT=arm -sTOOLSET_ROOT="%LINARO_ARM_GNUABIHF_2013_11_R1%\bin\arm-linux-gnueabihf-"   
)

call jam.bat -sBRAVO_TOP=%SolutionDir%\..\..\ -q -j4 %ProjectName%
