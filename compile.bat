@echo off
echo Compilando cliente...
gcc client.c -o ../bin/client.exe -lws2_32 -I../shared
if %ERRORLEVEL% EQU 0 (
    echo ✓ Cliente compilado com sucesso!
) else (
    echo ❌ Erro na compilação do cliente!
)
pause
