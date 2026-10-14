# Verificación de la entrega

Fecha: 13 de octubre de 2026 (Santiago).

## Ejecutado

- Compilador oficial nativo VitaSDK: `arm-vita-eabi-gcc 15.2.0`.
- Archivo de herramientas: `sdk-core-2026.08.0`, bootstrap Windows x86_64.
- Bibliotecas: snapshot `packages-2026.08-snapshot-20260925.50.1`; libvita2d `0.0.0.r188.ga8f15ab-1`.
- Archivo del SDK comprobado mediante SHA-256 publicado en su release; bibliotecas comprobadas contra los hashes de la base `vita.db` del canal 2026.08. No se ejecutó la verificación de firma de vdpm, porque su proceso MSYS no pudo iniciarse. No se presenta esta extracción manual como instalación verificada por vdpm.
- Compilación y enlace de `main.c` y `game.c` con `-Wall -Wextra`, sin advertencias en la versión entregada.
- Conversión ELF → VELF → SELF y empaquetado VPK correctos.
- Cabecera ELF revisada: ARM, ELF32, EABI5 hard-float.
- Paquete inspeccionado: `eboot.bin`, `sce_sys/param.sfo` y avisos de libvita2d/Newlib.

`toolchain-lock.json` contiene las URL y SHA-256 exactas; `SDK-VERSION.txt` conserva revisiones del SDK. El script `build.ps1` es el camino de compilación usado realmente.

## Pendiente

- Ejecutar `tests/test_game.c`: preparado para movimiento, colisión, rango, enfriamiento, reaparición, experiencia, máximo nivel, recuperación ante guardado truncado y error de escritura. **No ejecutado**: Zig falló al localizar su ejecutable en este entorno y TinyCC no pudo cargarse por un error del sistema de Windows.
- Compilación por CMake/WSL: preparada, no ejecutada.
- Instalación y ejecución física en PS Vita 3.60: no hay consola conectada a esta sesión.
- Medir FPS, memoria, suspensión y reanudación en hardware.

Una compilación correcta no demuestra por sí sola que la aplicación funcione en la consola. Si aparece un error, anota su código, el paso en que ocurre y si se creó un volcado en `ux0:data`.
