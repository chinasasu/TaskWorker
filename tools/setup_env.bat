@echo OFF

@if "%SETUP_ENV%"=="done" goto :EOF
@set  SETUP_ENV=done

@set ENV_CURRENT_PATH=%cd%
@cd /d %~dp0

if /i "%PROCESSOR_IDENTIFIER:~0,3%" == "X86" (set zippath=7z/x32) else (set zippath=7z/x64)
@echo zippath=%zippath%

@set PATH=%~dp0%zippath%;%PATH%

@if not exist cmake3.5 (7z.exe x cmake-3.5.0-win32-x86.zip -ocmake3.5)

@set PATH=%~dp0;%~dp0cmake3.5\bin;%PATH%

@cd /d %ENV_CURRENT_PATH%

:EOF