@echo off
echo ╔════════════════════════════════════════╗
echo ║        COMPILANDO PROJETO              ║
echo ╚════════════════════════════════════════╝
echo.

echo [1/2] Compilando servidor...
gcc server/server.c -o bin/server.exe -lws2_32 -Ishared
if %ERRORLEVEL% NEQ 0 (
    echo ❌ Erro ao compilar servidor!
    pause
    exit /b 1
)

echo [2/2] Compilando cliente...
gcc client/client.c -o bin/client.exe -lws2_32 -Ishared
if %ERRORLEVEL% NEQ 0 (
    echo ❌ Erro ao compilar cliente!
    pause
    exit /b 1
)

echo.
echo ╔════════════════════════════════════════╗
echo ║     COMPILACAO CONCLUIDA!              ║
echo ╚════════════════════════════════════════╝
echo.
echo Executaveis gerados em bin/
echo.
echo PRÓXIMOS PASSOS:
echo 1. Execute: cd bin && run_server.bat
echo 2. Execute: cd bin && run_client.bat (em 2 terminais)
echo.
pause
