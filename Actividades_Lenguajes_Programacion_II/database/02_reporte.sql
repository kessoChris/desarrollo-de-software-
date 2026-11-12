-- Reporte con los campos solicitados en el ejemplo de la actividad.
SELECT numero_empleado,
       nombre_completo,
       TO_CHAR(fecha_nacimiento, 'DD/MM/YYYY') AS fecha_nacimiento,
       rfc,
       centro_de_trabajo,
       descripcion_puesto,
       es_directivo
FROM rrhh.v_reporte_empleados
ORDER BY numero_empleado;

