param([string]$Sdk=$env:VITASDK)
$ErrorActionPreference='Stop'
if(!$Sdk){throw 'Indica -Sdk con la carpeta de VitaSDK o define VITASDK.'}
$env:VITASDK=(Resolve-Path $Sdk).Path
$env:PATH="$env:VITASDK\bin;$env:PATH"
Push-Location $PSScriptRoot
try {
    New-Item -ItemType Directory -Force build | Out-Null
    & arm-vita-eabi-gcc -std=c11 -O2 -Wall -Wextra '-Wl,-q' src/main.c src/game.c -o build/rovita.elf -lvita2d -lSceDisplay_stub -lSceGxm_stub -lSceSysmodule_stub -lSceCtrl_stub -lScePgf_stub -lScePvf_stub -lSceCommonDialog_stub -lSceAppMgr_stub -lSceIofilemgr_stub -lfreetype -lpng16 -ljpeg -lbz2 -lz -lm
    if($LASTEXITCODE){throw 'Fallo de compilacion'}
    & vita-elf-create build/rovita.elf build/rovita.velf
    if($LASTEXITCODE){throw 'Fallo VELF'}
    & vita-make-fself -s build/rovita.velf build/eboot.bin
    if($LASTEXITCODE){throw 'Fallo SELF'}
    & vita-mksfoex -s TITLE_ID=ROVT00001 -s APP_VER=01.00 'RO Vita Prototype' build/param.sfo
    if($LASTEXITCODE){throw 'Fallo SFO'}
    & vita-pack-vpk -s build/param.sfo -b build/eboot.bin -a licenses=licenses build/rovita.vpk
    if($LASTEXITCODE){throw 'Fallo VPK'}
    Get-FileHash build/rovita.vpk -Algorithm SHA256
} finally {Pop-Location}

