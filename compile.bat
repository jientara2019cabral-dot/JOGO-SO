@echo off
echo Compilando servidor...
gcc server.c -o ../bin/server.exe -lws2_32 -I../shared
if %ERRORLEVEL% EQU 0 (
    echo ✓ Servidor compilado com sucesso!
) else (
    echo ❌ Erro na compilação do servidor!
)
pause
