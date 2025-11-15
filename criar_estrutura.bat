@echo off
echo ╔════════════════════════════════════════╗
echo ║   CRIANDO ESTRUTURA DO PROJETO         ║
echo ╚════════════════════════════════════════╝
echo.

echo Criando pastas...
echo.

if not exist "server" (
    mkdir server
    echo ✓ Pasta server\ criada
) else (
    echo ✓ Pasta server\ ja existe
)

if not exist "client" (
    mkdir client
    echo ✓ Pasta client\ criada
) else (
    echo ✓ Pasta client\ ja existe
)

if not exist "shared" (
    mkdir shared
    echo ✓ Pasta shared\ criada
) else (
    echo ✓ Pasta shared\ ja existe
)

if not exist "docs" (
    mkdir docs
    echo ✓ Pasta docs\ criada
) else (
    echo ✓ Pasta docs\ ja existe
)

if not exist "bin" (
    mkdir bin
    echo ✓ Pasta bin\ criada
) else (
    echo ✓ Pasta bin\ ja existe
)

echo.
echo ╔════════════════════════════════════════╗
echo ║    ESTRUTURA CRIADA COM SUCESSO!       ║
echo ╚════════════════════════════════════════╝
echo.
echo Estrutura final:
echo.
echo JogoDaVelha\
echo ├── server\      (codigo do servidor)
echo ├── client\      (codigo do cliente)
echo ├── shared\      (arquivos compartilhados)
echo ├── docs\        (documentacao)
echo └── bin\         (executaveis)
echo.
echo Agora salve os arquivos nas pastas corretas!
echo.
pause
