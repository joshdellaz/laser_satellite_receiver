@echo off
call setEnv.bat
"D:\MATLAB_2021\toolbox\shared\coder\ninja\win64\ninja.exe" -t compdb cc cxx cudac > compile_commands.json
"D:\MATLAB_2021\toolbox\shared\coder\ninja\win64\ninja.exe" -v %*
