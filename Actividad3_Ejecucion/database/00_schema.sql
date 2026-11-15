-- Esquema relacional PostgreSQL para el control de empleados.
CREATE SCHEMA IF NOT EXISTS rrhh;

CREATE TABLE IF NOT EXISTS rrhh.centros_trabajo (
    centro_id       CHAR(6) PRIMARY KEY,
    nombre          VARCHAR(100) NOT NULL,
    ciudad          VARCHAR(80) NOT NULL
);

CREATE TABLE IF NOT EXISTS rrhh.puestos (
    puesto_id       INTEGER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    nombre          VARCHAR(80) NOT NULL UNIQUE,
    descripcion     VARCHAR(240) NOT NULL
);

CREATE TABLE IF NOT EXISTS rrhh.empleados (
    numero_empleado INTEGER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    nombre          VARCHAR(80) NOT NULL,
    apellido_paterno VARCHAR(80) NOT NULL,
    apellido_materno VARCHAR(80) NOT NULL,
    fecha_nacimiento DATE NOT NULL,
    centro_id       CHAR(6) NOT NULL REFERENCES rrhh.centros_trabajo(centro_id),
    puesto_id       INTEGER NOT NULL REFERENCES rrhh.puestos(puesto_id),
    es_directivo    BOOLEAN NOT NULL DEFAULT FALSE,
    rfc CHAR(13) GENERATED ALWAYS AS (
        UPPER(
            LEFT(REGEXP_REPLACE(TRANSLATE(apellido_paterno, 'ÁÉÍÓÚÜÑáéíóúüñ', 'AEIOUUNaeiouun'), '[^A-Za-z]', '', 'g'), 2) ||
            LEFT(REGEXP_REPLACE(TRANSLATE(apellido_materno, 'ÁÉÍÓÚÜÑáéíóúüñ', 'AEIOUUNaeiouun'), '[^A-Za-z]', '', 'g'), 1) ||
            LEFT(REGEXP_REPLACE(TRANSLATE(nombre, 'ÁÉÍÓÚÜÑáéíóúüñ', 'AEIOUUNaeiouun'), '[^A-Za-z]', '', 'g'), 1) ||
            RIGHT(EXTRACT(YEAR FROM fecha_nacimiento)::INTEGER::TEXT, 2) ||
            LPAD(EXTRACT(MONTH FROM fecha_nacimiento)::INTEGER::TEXT, 2, '0') ||
            LPAD(EXTRACT(DAY FROM fecha_nacimiento)::INTEGER::TEXT, 2, '0') ||
            'XXX'
        )
    ) STORED,
    UNIQUE (nombre, apellido_paterno, apellido_materno, fecha_nacimiento)
);

CREATE TABLE IF NOT EXISTS rrhh.directivos (
    numero_empleado INTEGER PRIMARY KEY
        REFERENCES rrhh.empleados(numero_empleado) ON DELETE CASCADE,
    centro_supervisado_id CHAR(6) NOT NULL
        REFERENCES rrhh.centros_trabajo(centro_id),
    apoyo_combustible BOOLEAN NOT NULL DEFAULT FALSE
);

CREATE OR REPLACE VIEW rrhh.v_reporte_empleados AS
SELECT e.numero_empleado,
       e.nombre || ' ' || e.apellido_paterno || ' ' || e.apellido_materno AS nombre_completo,
       e.fecha_nacimiento,
       e.rfc,
       c.nombre AS centro_de_trabajo,
       p.descripcion AS descripcion_puesto,
       e.es_directivo
FROM rrhh.empleados AS e
JOIN rrhh.centros_trabajo AS c USING (centro_id)
JOIN rrhh.puestos AS p USING (puesto_id);

