# Actividad 3 — Ejecución de datos

Jesús Christopher Enríquez Sotelo · Lenguajes de Programación II

Aplicación C++17 que consulta `personal_uni` por ODBC y muestra los datos de la Actividad 1 en tablas de consola. Continúa la conexión realizada en la Actividad 2. El entorno de edición es Visual Studio Code con GCC UCRT64, PostgreSQL 18 y el controlador PostgreSQL ODBC Driver(UNICODE) de 64 bits.

## Compilar y ejecutar

1. Abrir esta carpeta en VS Code. Tener PostgreSQL activo en `127.0.0.1:5432`, base `personal_uni`, y MSYS2 UCRT64 instalado en `C:/msys64`.
2. Ejecutar la tarea **Compilar Actividad 3** (Ctrl+Shift+B).
3. Ejecutar la tarea **Ejecutar Actividad 3 con datos A1**. Solicita usuario y contraseña oculta; Enter selecciona `postgres`. La opción `--cargar-a1` ejecuta el esquema y la carga idempotente de A1 dentro de una transacción. No elimina tablas ni registros.
4. Para consultar sin cargar datos, desde la carpeta del proyecto ejecutar `./build/empleados_odbc.exe`. Para repetir la carga: `./build/empleados_odbc.exe --cargar-a1`.

Compilación alternativa desde PowerShell, con `C:/msys64/ucrt64/bin` en PATH:

```powershell
g++ -std=c++17 -static -Wall -Wextra -DNOMINMAX -DWIN32_LEAN_AND_MEAN src/main.cpp -lodbc32 -o build/empleados_odbc.exe
```

## Componentes

- `src/db.hpp`: conexión Unicode, consultas SQL, SQLFetch y SQLGetData, diagnóstico y liberación de recursos ODBC.
- `src/modelo.hpp`: clases Empleado y Directivo, herencia, bandera virtual y consulta con JOIN.
- `src/tablas.hpp`: bordes y columnas, ajuste de textos largos y exportación de la base.
- `src/main.cpp`: autenticación local, carga opcional, consulta y presentación de resultados.
- `database/00_schema.sql` y `01_seed.sql`: esquema y datos de A1.
- `database/04_respaldo_a3.sql`: respaldo de las cuatro tablas y la vista, generado consultando la base real. Incluye identificadores y restablece secuencias.
- `evidencias/ejecucion.txt`: salida real completa; `tabla_1.txt` a `tabla_6.txt` conservan cada tabla. Las capturas muestran estos archivos en VS Code y el código utilizado.

La ejecución del 14 de noviembre de 2026 produjo 6 centros, 5 puestos, 5 empleados y 2 directivos. Se muestran los 5 empleados en un reporte de siete columnas, incluidos los tres empleados no directivos mediante LEFT JOIN.

## Restaurar el respaldo en pgAdmin 4

Crear una base NUEVA y vacía, por ejemplo `personal_uni_a3_revision`, con codificación UTF8. Abrir Query Tool sobre ella, cargar `database/04_respaldo_a3.sql` y ejecutar. El archivo recrea el esquema `rrhh`, las tablas, la vista y los datos capturados. No ejecutarlo sobre tablas ya pobladas: conserva las claves originales y los duplicados producirían un error. La transacción evita una restauración parcial.

## Alcance

El RFC conserva el cálculo académico de A1: prefijo de nombres, fecha y sufijo `XXX`; no representa un RFC validado oficialmente. Las fechas se muestran como DD/MM/YYYY. Los códigos de centro conservan seis dígitos; la clase Directivo usa un entero para el número supervisado y recupera los ceros al imprimir.

Las credenciales se solicitan en cada ejecución, nunca se guardan en el repositorio. La entrada oculta de esta versión admite caracteres ASCII. Usar un usuario y contraseña sin punto y coma en la cadena ODBC. Las salidas se consultan en una transacción de lectura con instantánea consistente.
