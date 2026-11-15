#pragma once
#include "db.hpp"
#include <memory>

class Empleado {
protected:
    int numero;
    std::string nombre, paterno, materno, nacimiento, rfc;
    std::string centro, puesto, descripcion;
public:
    explicit Empleado(const Row& r):numero(std::stoi(r.at(0))),
        nombre(r.at(1)),paterno(r.at(2)),materno(r.at(3)),nacimiento(r.at(4)),
        rfc(r.at(5)),centro(r.at(6)),puesto(r.at(7)),descripcion(r.at(8)) {}
    virtual ~Empleado()=default;
    virtual bool esDirectivo() const { return false; }
    Row reporte() const {
        return {std::to_string(numero),nombre+" "+paterno+" "+materno,
            nacimiento,rfc,centro,descripcion,esDirectivo()?"Sí":"No"};
    }
};

class Directivo final: public Empleado {
    int numeroCentroSupervisado;
    bool combustible;
public:
    explicit Directivo(const Row& r):Empleado(r),
        numeroCentroSupervisado(std::stoi(r.at(10))),combustible(r.at(11)=="1") {}
    bool esDirectivo() const override { return true; }
    Row detalle() const {
        auto clave=std::to_string(numeroCentroSupervisado);
        clave=std::string(6-clave.size(),'0')+clave;
        return {std::to_string(numero),nombre+" "+paterno+" "+materno,
                clave,combustible?"Sí":"No"};
    }
};

inline std::vector<std::unique_ptr<Empleado>> cargarEmpleados(Database& db) {
    auto rows=db.query(R"SQL(
        SELECT e.numero_empleado,e.nombre,e.apellido_paterno,e.apellido_materno,
               to_char(e.fecha_nacimiento,'DD/MM/YYYY'),e.rfc,c.nombre,
               p.nombre,p.descripcion,CASE WHEN e.es_directivo THEN 1 ELSE 0 END,
               d.centro_supervisado_id,CASE WHEN d.apoyo_combustible THEN 1 ELSE 0 END
        FROM rrhh.empleados e
        JOIN rrhh.centros_trabajo c USING(centro_id)
        JOIN rrhh.puestos p USING(puesto_id)
        LEFT JOIN rrhh.directivos d USING(numero_empleado)
        ORDER BY e.numero_empleado
    )SQL");
    std::vector<std::unique_ptr<Empleado>> empleados;
    for(const auto& row:rows) {
        if(row.at(9)=="1") {
            if(row.at(10).empty()) throw std::runtime_error("Directivo sin centro supervisado");
            empleados.push_back(std::make_unique<Directivo>(row));
        } else empleados.push_back(std::make_unique<Empleado>(row));
    }
    return empleados;
}
