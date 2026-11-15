-- Respaldo de rrhh obtenido desde PostgreSQL por ODBC.
-- Restaurar en una base NUEVA y vacía con codificación UTF8.
BEGIN;
SET standard_conforming_strings=on;
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


INSERT INTO rrhh.centros_trabajo (centro_id,nombre,ciudad) VALUES ('000201','Tiendas Ángel Flores Ropa','Culiacán');
INSERT INTO rrhh.centros_trabajo (centro_id,nombre,ciudad) VALUES ('000202','Tiendas Ángel Flores Muebles','Culiacán');
INSERT INTO rrhh.centros_trabajo (centro_id,nombre,ciudad) VALUES ('000203','Tiendas Ángel Flores Cajas','Culiacán');
INSERT INTO rrhh.centros_trabajo (centro_id,nombre,ciudad) VALUES ('049001','La Primavera Ropa','Culiacán');
INSERT INTO rrhh.centros_trabajo (centro_id,nombre,ciudad) VALUES ('049002','La Primavera Muebles','Culiacán');
INSERT INTO rrhh.centros_trabajo (centro_id,nombre,ciudad) VALUES ('049003','La Primavera Cajas','Culiacán');
INSERT INTO rrhh.puestos (puesto_id,nombre,descripcion) OVERRIDING SYSTEM VALUE VALUES ('1','Vendedor','Atiende clientes y registra las ventas de su área.');
INSERT INTO rrhh.puestos (puesto_id,nombre,descripcion) OVERRIDING SYSTEM VALUE VALUES ('2','Cajero','Realiza cobros, cortes y control de caja.');
INSERT INTO rrhh.puestos (puesto_id,nombre,descripcion) OVERRIDING SYSTEM VALUE VALUES ('3','Auxiliar administrativo','Apoya el registro y seguimiento administrativo.');
INSERT INTO rrhh.puestos (puesto_id,nombre,descripcion) OVERRIDING SYSTEM VALUE VALUES ('4','Gerente de tienda','Coordina personal, operación y resultados de la tienda.');
INSERT INTO rrhh.puestos (puesto_id,nombre,descripcion) OVERRIDING SYSTEM VALUE VALUES ('5','Director de zona','Supervisa centros de trabajo y coordina a sus gerentes.');
INSERT INTO rrhh.empleados (numero_empleado,nombre,apellido_paterno,apellido_materno,fecha_nacimiento,centro_id,puesto_id,es_directivo) OVERRIDING SYSTEM VALUE VALUES ('1','Jesús','Vega','Castro','1988-03-26','000201','4','1');
INSERT INTO rrhh.empleados (numero_empleado,nombre,apellido_paterno,apellido_materno,fecha_nacimiento,centro_id,puesto_id,es_directivo) OVERRIDING SYSTEM VALUE VALUES ('2','José','López','Pérez','1980-01-01','000202','1','0');
INSERT INTO rrhh.empleados (numero_empleado,nombre,apellido_paterno,apellido_materno,fecha_nacimiento,centro_id,puesto_id,es_directivo) OVERRIDING SYSTEM VALUE VALUES ('3','María','García','Torres','1992-07-14','000203','2','0');
INSERT INTO rrhh.empleados (numero_empleado,nombre,apellido_paterno,apellido_materno,fecha_nacimiento,centro_id,puesto_id,es_directivo) OVERRIDING SYSTEM VALUE VALUES ('4','Luis','Hernández','Ramos','1990-11-02','049001','3','0');
INSERT INTO rrhh.empleados (numero_empleado,nombre,apellido_paterno,apellido_materno,fecha_nacimiento,centro_id,puesto_id,es_directivo) OVERRIDING SYSTEM VALUE VALUES ('5','Ana Sofía','López','Cruz','1995-05-19','049002','5','1');
INSERT INTO rrhh.directivos (numero_empleado,centro_supervisado_id,apoyo_combustible) VALUES ('1','000202','1');
INSERT INTO rrhh.directivos (numero_empleado,centro_supervisado_id,apoyo_combustible) VALUES ('5','049003','0');
SELECT setval(pg_get_serial_sequence('rrhh.puestos','puesto_id'), COALESCE(MAX(puesto_id),1), MAX(puesto_id) IS NOT NULL) FROM rrhh.puestos;
SELECT setval(pg_get_serial_sequence('rrhh.empleados','numero_empleado'), COALESCE(MAX(numero_empleado),1), MAX(numero_empleado) IS NOT NULL) FROM rrhh.empleados;
COMMIT;
