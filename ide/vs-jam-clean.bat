set SolutionDir=%~dp0
set Config=%1
set Top=%SolutionDir%..\..\

cd %Top%
call jam.bat clean
cd %Top%
del Jamrules.%Config%
