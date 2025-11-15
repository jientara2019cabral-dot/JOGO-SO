@echo off
echo ╔════════════════════════════════════════╗
echo ║   CRIANDO ESTRUTURA DO PROJETO         ║
echo ╚════════════════════════════════════════╝
echo.

echo Criando pastas...
if not exist "server" mkdir server
if not exist "client" mkdir client
if not exist "shared" mkdir shared
if not exist "bin" mkdir bin

echo.
echo ✓ Estrutura criada com sucesso!
echo.
echo JogoDaVelha/
echo ├── server/
echo ├── client/
echo ├── shared/ 
echo └── bin/
echo.
pause
