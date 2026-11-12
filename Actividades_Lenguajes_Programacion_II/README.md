# Sistema de empleados - PostgreSQL y C++ ODBC

Proyecto didáctico para las actividades 1 y 2 de Lenguajes de Programación II.

## Requisitos

- PostgreSQL 16 o posterior y pgAdmin 4.
- Controlador ODBC Unicode de PostgreSQL (psqlODBC) de 64 bits.
- Visual Studio con la carga de trabajo **Desarrollo para el escritorio con C++**, o CMake con un compilador C++ compatible.

## Actividad 1: modelo, esquema y datos de ejemplo

1. En pgAdmin, crea la base de datos `personal_uni`.
2. Abre Query Tool en esa base.
3. Ejecuta `database/00_schema.sql` para crear las tablas y restricciones.
4. Ejecuta `database/01_seed.sql` para cargar los seis centros y cinco empleados de demostración.
5. Ejecuta `database/02_reporte.sql` para mostrar el reporte solicitado.

Los empleados incluidos son datos ficticios. El RFC se calcula con las cuatro iniciales, la fecha y el sufijo didáctico `XXX`; no incluye homoclave oficial y no sirve para trámites.

## Actividad 2: conexión ODBC y tablas vacías

La aplicación obtiene la cadena de conexión de la variable de entorno `PERSONAL_UNI_ODBC`. En PowerShell, asígnala antes de ejecutar; reemplaza usuario y contraseña por los de tu instalación:

```powershell
$env:PERSONAL_UNI_ODBC = 'Driver={PostgreSQL ODBC Driver(UNICODE)};Server=127.0.0.1;Port=5432;Database=personal_uni;Uid=USUARIO;Pwd=CONTRASEÑA;'
```

Para conservar el esquema y dejar vacías las tablas antes de la demostración de la Actividad 2, ejecuta `database/03_vaciar_para_a2.sql` en `personal_uni`. Este script elimina los datos de ejemplo y reinicia identidades; úsalo únicamente en esta base didáctica.

Abre `empleados_odbc.sln` en Visual Studio, selecciona **Release | x64** y compila la solución. También puedes compilar con CMake si está instalado:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

Después ejecuta `build/Release/empleados_odbc.exe`. La salida imprime el encabezado de cada tabla y una leyenda de tabla vacía; las consultas usan `WHERE 1 = 0`, por lo que nunca muestran filas.

## Archivos

- `database/00_schema.sql`: tablas, claves y vista del reporte.
- `database/01_seed.sql`: catálogos y datos ficticios para A1.
- `database/02_reporte.sql`: consulta del reporte.
- `database/03_vaciar_para_a2.sql`: reinicio controlado para A2.
- `src/main.cpp`: conexión ODBC y salida de encabezados vacíos.
- `CMakeLists.txt`: configuración de compilación en Windows.

