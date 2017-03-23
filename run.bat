@echo off
cls

rem target settings
set TARGET_DIR=build
set TARGET=test

if exist .\%TARGET_DIR%\%TARGET%.exe .\%TARGET_DIR%\%TARGET%.exe
if not exist .\%TARGET_DIR%\%TARGET%.exe build