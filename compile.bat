@echo off
echo ========================================
echo  COMPILANDO SERVIDOR
echo ========================================
echo.

cd /d "%~dp0"

if not exist "..\bin" mkdir "..\bin"

gcc server.c -o ..\bin\server.exe -lws2_32 -I..\shared

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✓ Servidor compilado com sucesso!
    echo ✓ Executavel: bin\server.exe
    echo.
) else (
    echo.
    echo ✗ Erro na compilacao!
    echo.
)

pause
