@echo off
setlocal enabledelayedexpansion

:: Definir rutas
set "source=.."
set "destination=..\backup.zip"
set "tempDir=%temp%\backupTemp"
set "originalPremake=%source%\premake5.lua"
set "tempPremake=%tempDir%\premake5.lua"

echo Creando archivo ZIP con todo el contenido de la raíz en %destination%, excluyendo las carpetas build y src, y modificando el archivo premake5.lua...

:: Crear directorio temporal
if not exist "%tempDir%" mkdir "%tempDir%"

powershell -Command "$content = Get-Content -Path '..\premake5.lua' -Raw; $pattern = '(?ms)^-----.*?project ""Motor"".*?^-----'; $newContent = $content -replace $pattern, ''; $newContent = $newContent -replace '^\s*\n', ''; $newContent | Set-Content -Path '%tempPremake%' -NoNewline"


:: Comprimir todo el contenido de la raíz, excluyendo las carpetas build y src, y el archivo premake5.lua original
powershell -command "$sourcePath = '%source%'; $destinationPath = '%destination%'; $tempDirPath = '%tempDir%'; $excludeItems = 'build', 'src', 'premake5.lua'; Get-ChildItem -Path $sourcePath -Exclude $excludeItems | Compress-Archive -DestinationPath $destinationPath -Force; Compress-Archive -Path $tempDirPath\premake5.lua -Update -DestinationPath $destinationPath"

:: Eliminar el directorio temporal
rmdir /s /q "%tempDir%"

echo Completado.
pause
