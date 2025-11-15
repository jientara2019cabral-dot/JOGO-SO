@echo off
echo ╔════════════════════════════════════════╗
echo ║  JOGO DA VELHA - COMPILACAO COMPLETA  ║
echo ╚════════════════════════════════════════╝
echo.

REM Verificar se o GCC está instalado
where gcc >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ❌ ERRO: GCC nao encontrado!
    echo.
    echo Por favor, instale o MinGW-w64 ou TDM-GCC
    echo Download: https://jmeubank.github.io/tdm-gcc/
    echo.
    pause
    exit /b 1
)

echo ✓ GCC encontrado
echo.

REM Criar pasta bin se não existir
if not exist "bin" mkdir bin
echo ✓ Pasta bin verificada
echo.

echo ----------------------------------------
echo [1/2] Compilando SERVIDOR...
echo ----------------------------------------
gcc server\server.c -o bin\server.exe -lws2_32 -I.\shared

if %ERRORLEVEL% NEQ 0 (
    echo ❌ Erro ao compilar servidor!
    pause
    exit /b 1
)
echo ✓ Servidor compilado com sucesso!
echo.

echo ----------------------------------------
echo [2/2] Compilando CLIENTE...
echo ----------------------------------------
gcc client\client.c -o bin\client.exe -lws2_32 -I.\shared

if %ERRORLEVEL% NEQ 0 (
    echo ❌ Erro ao compilar cliente!
    pause
    exit /b 1
)
echo ✓ Cliente compilado com sucesso!
echo.

echo ╔════════════════════════════════════════╗
echo ║     COMPILACAO CONCLUIDA COM SUCESSO!  ║
echo ╚════════════════════════════════════════╝
echo.
echo Arquivos gerados:
dir /b bin\*.exe
echo.
echo ----------------------------------------
echo  COMO EXECUTAR:
echo ----------------------------------------
echo  1. Abra um terminal e execute:
echo     cd bin
echo     server.exe
echo.
echo  2. Abra outro terminal e execute:
echo     cd bin
echo     client.exe
echo.
echo  3. Abra um terceiro terminal e execute:
echo     cd bin
echo     client.exe
echo ----------------------------------------
echo.
pause
