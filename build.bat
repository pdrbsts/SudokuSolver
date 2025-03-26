@echo off
echo Building Sudoku.exe with MSVC...
del Sudoku.exe

REM Set up the environment for MSVC (you may need to adjust the path)
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

REM Compile resources
rc.exe resources.rc

REM Compile the program
cl.exe /EHsc main.c resources.res /FeSudoku.exe

echo Build completed successfully!
del main.obj
pause
