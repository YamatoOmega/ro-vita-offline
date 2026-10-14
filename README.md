# RO Vita — prototipo offline 0.1

Base nativa en C para PS Vita, orientada al futuro proyecto Pre-Renewal. No es todavía un port completo de Ragnarok ni reproduce las reglas de XatiyaRO. No necesita cliente de RO, servidor, base de datos ni conexión para jugar.

## Probar en PS Vita 3.60 con HENkaku Enso

1. Copia `build/rovita.vpk` a `ux0:data/rovita.vpk` mediante VitaShell. Puedes utilizar su conexión USB desde Windows; si prefieres FTP, usa la dirección que muestre VitaShell en tu red local.
2. Desconecta correctamente el USB o termina la transferencia. En VitaShell abre el VPK y confirma su instalación.
3. Abre la burbuja **RO Vita Prototype**. El identificador es `ROVT00001`; reinstalar una versión con el mismo identificador actualiza esa aplicación.
4. Debes ver un terreno verde, obstáculos, un personaje azul y una criatura rosa llamada Poring. Puedes desconectar la red para comprobar que funciona offline.
5. Rodea el obstáculo central, acércate al Poring y mantén X para atacar. Tras dos derrotas deberías alcanzar el nivel 2.
6. Pulsa Triángulo: debe aparecer **Guardado OK**. Pulsa START para guardar y salir, vuelve a abrir y comprueba posición, nivel, EXP y contador de Porings.

No hace falta actualizar el firmware ni cambiar Enso. El ejecutable se empaqueta en modo seguro (`vita-make-fself -s`), sin módulos de kernel. La compatibilidad real con tu consola todavía requiere esta prueba física. No se ha medido rendimiento en hardware.

| Control | Acción |
|---|---|
| Stick izquierdo o cruceta | Movimiento con colisiones |
| X, mantenida o pulsada | Ataque radial a 64 píxeles; intervalo de 0,4 s |
| Triángulo | Guardar |
| START | Guardar y salir; si falla el guardado, permanece abierto |
| PS | Menú del sistema; no garantiza un guardado inmediato |

Se guarda cada 10 segundos y al derrotar un Poring, además de los controles manuales. Los archivos `ux0:data/rovita/save.0` y `save.1` alternan versiones con suma de comprobación. Se recupera la versión válida más reciente; si ambas están dañadas se inicia una nueva partida. Esto reduce el riesgo de pérdida, pero no garantiza recuperación ante cualquier corte de energía. Copia ambos archivos antes de experimentar. Para reiniciar, respáldalos y luego bórralos con la aplicación cerrada. No se guarda la salud del monstruo: reaparece al cargar.

## Contenido y alcance

- Mapa temporal de 30 × 17 celdas, mostrado a 960 × 544, con límites y dos obstáculos.
- Movimiento independiente de la tasa de cuadros, diagonales normalizadas y zona muerta del stick.
- Poring temporal inmóvil: 30 HP, reaparición a los 3 segundos. No ataca al jugador.
- Daño provisional `10 + 2 × nivel`; cada derrota da 15 EXP.
- EXP provisional por nivel `20 + 10 × nivel`; límite de personaje 99.
- Personaje, terreno y criatura dibujados con primitivas; sin texturas, música ni fuentes copiadas. La fuente se carga del sistema de la Vita.

Estas cifras sirven para probar el ciclo de juego; **no son fórmulas Pre-Renewal verificadas**. Todavía faltan jobs, clases, inventario, habilidades, equipo, muerte del personaje, IA, mapas originales y cámara isométrica.

## Estructura

```
src/game.c,h       lógica portable, colisiones, combate, progreso y guardado
src/main.c         controles Vita, reloj, renderizado y mensajes
tests/test_game.c  pruebas de la lógica en PC
licenses/         avisos de componentes incluidos
build.ps1         compilación nativa desde PowerShell
CMakeLists.txt    alternativa para WSL/Linux con VitaSDK
build/            VPK y archivos de diagnóstico de esta compilación
```

## Volver a compilar en este PC

Abre PowerShell en la carpeta de este README y ejecuta:

```powershell
./build.ps1 -Sdk ../../work/vitasdk
```

Esta ruta corresponde a la carpeta de trabajo entregada en esta conversación. Si mueves el proyecto, indica la nueva ruta de VitaSDK. No se modificó el PATH permanente de Windows. La ruta nativa fue compilada realmente; CMake es una alternativa preparada, no ejecutada en esta sesión.

## Instalar las herramientas en otro equipo

VitaSDK recomienda WSL2. En Windows 10, sigue primero la [instalación oficial de WSL](https://learn.microsoft.com/windows/wsl/install), que puede requerir administrador y reinicio. Dentro de Ubuntu:

```sh
sudo apt update
sudo apt install build-essential git cmake curl python3
export VITASDK="$HOME/vitasdk"
export PATH="$VITASDK/bin:$PATH"
git clone https://github.com/vitasdk/vdpm
cd vdpm
./bootstrap-vitasdk.sh
vdpm install libvita2d
vdpm status
# Desde la carpeta del proyecto:
cmake -S . -B build-wsl
cmake --build build-wsl --parallel
```

Guarda las dos líneas `export` en tu configuración de shell si deseas conservarlas entre sesiones. El resultado será `build-wsl/rovita.vpk`.

La [documentación actual de VitaSDK](https://vitasdk.org/) también ofrece `bootstrap-vitasdk.ps1` en el [repositorio vdpm](https://github.com/vitasdk/vdpm) para Windows nativo. El README histórico de libvita2d menciona `install-all.sh`; para una instalación nueva usa el gestor actual: `vdpm install libvita2d`.

En esta sesión el gestor Pacman/MSYS no pudo iniciarse por una restricción del entorno. Para compilar se extrajo el archivo oficial nativo `sdk-core-2026.08.0`, se comprobaron hashes y se agregaron bibliotecas del canal 2026.08. No se desactivaron las comprobaciones del instalador. Esta extracción es un entorno portátil de compilación; **no equivale a una instalación administrada por vdpm**. Consulta `BUILD-INFO.md` para las versiones exactas. Los enlaces simbólicos auxiliares de libpng no se pudieron extraer en Windows; se enlaza directamente `png16`, que sí se extrajo correctamente.

## Pruebas en PC

Con un compilador C de escritorio, desde la raíz del proyecto:

```sh
cc -std=c11 -Wall -Wextra -Werror src/game.c tests/test_game.c -lm -o test_game
./test_game
```

Con Zig en Windows: `zig cc -std=c11 -Wall -Wextra -Werror src/game.c tests/test_game.c -o test_game.exe`, luego `./test_game.exe`. Las pruebas crean y borran únicamente `test.save.0` y `test.save.1` en la carpeta actual; no uses esos nombres para partidas propias.

## Próximas etapas Pre-Renewal

1. Validar instalación, controles, lectura de texto, suspensión/reanudación y guardado en la Vita 3.60. Registrar errores y rendimiento antes de ampliar contenido.
2. Fijar una revisión de [rAthena](https://github.com/rathena/rathena) como referencia; revisar `src/config/renewal.hpp` y `db/pre-re`. Definir reglas concretas de la época deseada y las diferencias conocidas de XatiyaRO. No asumir que sus rates o ajustes privados son idénticos a rAthena.
3. Separar datos de monstruos y curvas de experiencia en tablas locales, añadir HP del jugador, defensa, precisión y ASPD, con pruebas contra casos conocidos.
4. Añadir Novice y primera clase; después segundas clases y renacidas. Límite objetivo 99 de base y 70 de job para renacidas; sin terceras clases.
5. Inventario, equipo y habilidades por etapas. Mantener toda simulación dentro del proceso: no ejecutar los servidores login/char/map ni MySQL en la consola.
6. Estudiar formatos en [roBrowserLegacy](https://github.com/MrAntares/roBrowserLegacy), evaluar un conversor de recursos autorizado en PC y cargar solo lo necesario para cada mapa. Medir memoria antes de adoptar mapas completos.

## Recursos y licencias

No se incorporó código ni bases de datos de rAthena o roBrowserLegacy. Ambos repositorios incluyen GPL-3.0: si se reutiliza su código, deben conservarse avisos y cumplirse las condiciones aplicables de distribución y código fuente. Una consulta como referencia no concede derechos sobre gráficos o música de terceros.

libvita2d se distribuye bajo MIT; se incluye su aviso junto con el de Newlib dentro del VPK y del proyecto. Los dibujos temporales se construyen en nuestro código; no se extrajeron GRF, sprites, mapas, sonidos ni logos. No distribuyas recursos comerciales sin permiso o una licencia que lo permita. Conserva para cada futuro asset su autor, origen y licencia; prioriza obras propias o recursos con licencia clara. El proyecto no está afiliado a Gravity ni a XatiyaRO. La licencia de publicación del código nuevo queda pendiente de tu decisión.

Fuentes revisadas el 13 de octubre de 2026: [VitaSDK](https://vitasdk.org/), [migración](https://vitasdk.org/migration/), [libvita2d](https://github.com/xerpi/libvita2d), [VitaShell](https://github.com/TheOfficialFloW/VitaShell), [licencia rAthena](https://github.com/rathena/rathena/blob/master/LICENSE), [licencia roBrowserLegacy](https://github.com/MrAntares/roBrowserLegacy/blob/master/LICENSE).

