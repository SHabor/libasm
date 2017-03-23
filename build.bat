@echo off

rem compiler settings
set TASM_PATH=.\tools\tasm
set COMPILER=%TASM_PATH%\TASM.EXE
set LINKER=%TASM_PATH%\TLINK.EXE
set DEBUGGER=%TASM_PATH%\TD.EXE

rem target settings
set TARGET_DIR=build
set TARGET=test

rem no need to clog source directory
if exist %TARGET_DIR% del %TARGET_DIR%\*.*
if not exist %TARGET_DIR% mkdir %TARGET_DIR%

rem list of source files
set OBJ=%TARGET_DIR%\%TARGET%.obj

%COMPILER% %TARGET%.asm %OBJ%
%LINKER% %OBJ%, %TARGET_DIR%\%TARGET%.exe

rem cls

%TARGET_DIR%\%TARGET%.exe