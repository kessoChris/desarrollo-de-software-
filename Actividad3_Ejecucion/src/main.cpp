#include <conio.h>
#include <filesystem>
#include "modelo.hpp"
#include "tablas.hpp"

int main(int argc,char** argv){
    SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8);
    std::filesystem::create_directories("evidencias");
    try{
        std::cout<<"ACTIVIDAD 3 - EJECUCIÓN DE DATOS - EMPRESA UNI\n";
        std::string user,password;
        std::cout<<"Usuario PostgreSQL [postgres]: "<<std::flush;
        std::getline(std::cin,user); if(user.empty())user="postgres";
        std::cout<<"Contraseña PostgreSQL (oculta): "<<std::flush;
        for(int key;(key=_getch())!='\r' && key!='\n';){
            if(key==8){if(!password.empty())password.pop_back();}
            else if(key>=32 && key<=126)password+=static_cast<char>(key);
        }
        std::cout<<'\n';
        Database db(user,password);
        SecureZeroMemory(password.data(),password.size()); password.clear();
        // Carga explícita e idempotente de los mismos datos de la Actividad 1.
        if(argc>1 && std::string(argv[1])=="--cargar-a1"){
            db.query("BEGIN");
            try{db.query(leer("database/00_schema.sql"));db.query(leer("database/01_seed.sql"));db.query("COMMIT");}
            catch(...){db.query("ROLLBACK");throw;}
        }
        // Todas las consultas del reporte comparten una instantánea consistente.
        db.query("BEGIN TRANSACTION ISOLATION LEVEL REPEATABLE READ READ ONLY");
        auto empleados=cargarEmpleados(db);
        Rows reporte,directivos;
        for(const auto& e:empleados){
            reporte.push_back(e->reporte());
            if(auto d=dynamic_cast<const Directivo*>(e.get()))directivos.push_back(d->detalle());
        }
        std::vector<std::string> salidas;
        salidas.push_back(tabla("CENTROS DE TRABAJO",{"Clave","Nombre del centro","Ciudad"},
            db.query("SELECT centro_id,nombre,ciudad FROM rrhh.centros_trabajo ORDER BY centro_id"),{6,32,15}));
        salidas.push_back(tabla("PUESTOS",{"ID","Puesto","Descripción"},
            db.query("SELECT puesto_id,nombre,descripcion FROM rrhh.puestos ORDER BY puesto_id"),{3,24,55}));
        salidas.push_back(tabla("EMPLEADOS - DATOS PERSONALES",{"Número","Nombre","Apellido paterno","Apellido materno","Nacimiento","RFC"},
            db.query("SELECT numero_empleado,nombre,apellido_paterno,apellido_materno,to_char(fecha_nacimiento,'DD/MM/YYYY'),rfc FROM rrhh.empleados ORDER BY numero_empleado"),{6,12,16,16,10,13}));
        salidas.push_back(tabla("EMPLEADOS - ASIGNACIÓN",{"Número","Centro","Puesto ID","Directivo 1/0"},
            db.query("SELECT numero_empleado,centro_id,puesto_id,CASE WHEN es_directivo THEN 1 ELSE 0 END FROM rrhh.empleados ORDER BY numero_empleado"),{6,6,9,13}));
        salidas.push_back(tabla("DIRECTIVOS",{"Número","Nombre completo","Centro supervisado","Combustible"},directivos,{6,28,18,11}));
        salidas.push_back(tabla("REPORTE GENERAL DE EMPLEADOS",{"Número","Nombre completo","Nacimiento","RFC","Nombre del centro","Descripción del puesto","Directivo"},
            reporte,{6,22,10,13,22,27,9}));
        respaldo(db); db.query("COMMIT");
        std::string all="Conexión ODBC a PostgreSQL establecida. Base: personal_uni\n\n";
        for(size_t i=0;i<salidas.size();++i){
            guardar("evidencias/tabla_"+std::to_string(i+1)+".txt",salidas[i]);
            all+=salidas[i]+"\n";
        }
        all+="Ejecución completada. Respaldo SQL generado desde los registros consultados.\n";
        guardar("evidencias/ejecucion.txt",all);
        std::cout<<all;
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"ERROR: "<<e.what()<<'\n';
        guardar("evidencias/error.txt",e.what());return 1;
    }
}
