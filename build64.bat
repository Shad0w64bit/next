@echo off
setlocal
:@SET PATH=%PATH%;"C:\Program Files (x86)\CodeBlocks\MinGW\bin"
cd /D "C:\Program Files (x86)\CodeBlocks\MinGW\bin"
@echo on
g++ "%~dp0main64.cpp" -o "%~dp0build64.exe" -Wall -Wextra -std=c++17
@pause