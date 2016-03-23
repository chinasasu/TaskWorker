@set CURRENTPATH=%cd%
@cd /d %~dp0

@call .\tools\setup_env.bat

@if not exist build md build
@cd build

cmake -G "Visual Studio 14 2015" ..

@cd /d %CURRENTPATH%