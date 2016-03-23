@echo OFF

@if "%SETUP_ENV%"=="done" goto :EOF
@set  SETUP_ENV=done

@set PATH=%~dp0cmake-3.5.0-win32-x86\bin;%PATH%

:EOF