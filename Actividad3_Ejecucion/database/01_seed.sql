-- Catálogo proporcionado en la consigna (conserva los ceros iniciales).
INSERT INTO rrhh.centros_trabajo (centro_id, nombre, ciudad) VALUES
('000201', 'Tiendas Ángel Flores Ropa', 'Culiacán'),
('000202', 'Tiendas Ángel Flores Muebles', 'Culiacán'),
('000203', 'Tiendas Ángel Flores Cajas', 'Culiacán'),
('049001', 'La Primavera Ropa', 'Culiacán'),
('049002', 'La Primavera Muebles', 'Culiacán'),
('049003', 'La Primavera Cajas', 'Culiacán')
ON CONFLICT (centro_id) DO NOTHING;

INSERT INTO rrhh.puestos (nombre, descripcion) VALUES
('Vendedor', 'Atiende clientes y registra las ventas de su área.'),
('Cajero', 'Realiza cobros, cortes y control de caja.'),
('Auxiliar administrativo', 'Apoya el registro y seguimiento administrativo.'),
('Gerente de tienda', 'Coordina personal, operación y resultados de la tienda.'),
('Director de zona', 'Supervisa centros de trabajo y coordina a sus gerentes.')
ON CONFLICT (nombre) DO NOTHING;

INSERT INTO rrhh.empleados
    (nombre, apellido_paterno, apellido_materno, fecha_nacimiento, centro_id, puesto_id, es_directivo)
SELECT datos.nombre, datos.apellido_paterno, datos.apellido_materno,
       datos.fecha_nacimiento, datos.centro_id, p.puesto_id, datos.es_directivo
FROM (VALUES
    (1, 'Jesús', 'Vega', 'Castro', DATE '1988-03-26', '000201', 'Gerente de tienda', TRUE),
    (2, 'José', 'López', 'Pérez', DATE '1980-01-01', '000202', 'Vendedor', FALSE),
    (3, 'María', 'García', 'Torres', DATE '1992-07-14', '000203', 'Cajero', FALSE),
    (4, 'Luis', 'Hernández', 'Ramos', DATE '1990-11-02', '049001', 'Auxiliar administrativo', FALSE),
    (5, 'Ana Sofía', 'López', 'Cruz', DATE '1995-05-19', '049002', 'Director de zona', TRUE)
) AS datos(orden, nombre, apellido_paterno, apellido_materno, fecha_nacimiento, centro_id, puesto, es_directivo)
JOIN rrhh.puestos AS p ON p.nombre = datos.puesto
ORDER BY datos.orden
ON CONFLICT (nombre, apellido_paterno, apellido_materno, fecha_nacimiento) DO NOTHING;

INSERT INTO rrhh.directivos (numero_empleado, centro_supervisado_id, apoyo_combustible)
SELECT e.numero_empleado, datos.centro_supervisado_id, datos.apoyo_combustible
FROM (VALUES
    ('Jesús', 'Vega', 'Castro', DATE '1988-03-26', '000202', TRUE),
    ('Ana Sofía', 'López', 'Cruz', DATE '1995-05-19', '049003', FALSE)
) AS datos(nombre, apellido_paterno, apellido_materno, fecha_nacimiento,
           centro_supervisado_id, apoyo_combustible)
JOIN rrhh.empleados AS e USING (nombre, apellido_paterno, apellido_materno, fecha_nacimiento)
ON CONFLICT (numero_empleado) DO NOTHING;

