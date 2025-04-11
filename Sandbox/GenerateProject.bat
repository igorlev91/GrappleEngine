@echo off
pushd %~dp0\..\Sandbox\
call ..\vendor\Premake\bin\premake5.exe vs2022 --Grapple_root=../ --file=Build.lua
popd
PAUSE
