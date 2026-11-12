# Sistema de empleados — PostgreSQL, C++ y Visual Studio Code

Proyecto de las actividades 1 y 2 de Lenguajes de Programación II. La Actividad 2 usa Visual Studio Code con C++ (GCC UCRT64 de MSYS2) y se conecta por ODBC a PostgreSQL.

## Requisitos para la Actividad 2

- PostgreSQL y pgAdmin 4, con la base `personal_uni` creada en la Actividad 1.
- Controlador ODBC Unicode de PostgreSQL (psqlODBC) de 64 bits.
- Visual Studio Code y la extensión **C/C++** de Microsoft.
- MSYS2 con el compilador GCC de UCRT64 y GDB.

En la terminal MSYS2 UCRT64, instalar el conjunto de herramientas con:

```bash
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
```

La tarea del proyecto usa `C:\msys64\ucrt64\bin\g++.exe`. Si MSYS2 está en otra carpeta, actualiza `compilerPath` en `.vscode/c_cpp_properties.json` y el compilador en `.vscode/tasks.json`.

## Actividad 1: esquema y datos de ejemplo

1. En pgAdmin, crea `personal_uni` si aún no existe.
2. Abre Query Tool en esa base y ejecuta `database/00_schema.sql`.
3. Ejecuta `database/01_seed.sql` para insertar centros, puestos y empleados ficticios.
4. Ejecuta `database/02_reporte.sql` para consultar el reporte.

El RFC es demostrativo: usa iniciales, fecha y sufijo `XXX`; no incluye homoclave oficial ni sirve para trámites.

## Actividad 2: conexión ODBC y encabezados vacíos

1. Confirma que PostgreSQL esté iniciado y abre `personal_uni` en pgAdmin.
2. Para restablecer el escenario vacío, ejecuta `database/03_vaciar_para_a2.sql` **solo** en `personal_uni`. Conserva las tablas y elimina los registros de demostración.
3. Abre esta carpeta en Visual Studio Code.
4. En VS Code, ejecuta **Terminal > Run Build Task** para compilar `src/main.cpp`.
5. Abre un terminal integrado de PowerShell y ejecuta `$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH; .\build\empleados_odbc.exe`. El programa solicita el usuario (por defecto `postgres`) y pide la contraseña sin mostrarla en pantalla. No guardes la contraseña en el código ni en GitHub.
6. La aplicación se conecta a `personal_uni` y consulta las cuatro tablas con `WHERE 1 = 0`. ODBC devuelve los metadatos, así que se imprimen únicamente los encabezados.

Las tareas para compilar y ejecutar, la configuración de IntelliSense y la depuración están en `.vscode/`. También puedes ejecutar en la terminal de PowerShell:

```powershell
& 'C:\msys64\ucrt64\bin\g++.exe' -std=c++17 -Wall -Wextra -DNOMINMAX -DWIN32_LEAN_AND_MEAN src/main.cpp -lodbc32 -o build/empleados_odbc.exe
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
& '.\build\empleados_odbc.exe'
```

## Archivos

- `database/00_schema.sql`: esquema, tablas, claves y vista del reporte.
- `database/01_seed.sql`: catálogos y datos ficticios para la Actividad 1.
- `database/02_reporte.sql`: consulta del reporte.
- `database/03_vaciar_para_a2.sql`: reinicio controlado para la práctica de tablas vacías.
- `src/main.cpp`: conexión ODBC y salida de encabezados en C++.
- `.vscode/tasks.json`: compilación y ejecución desde Visual Studio Code.
- `.vscode/c_cpp_properties.json`: IntelliSense con GCC UCRT64.
- `.vscode/launch.json`: depuración con GDB.

