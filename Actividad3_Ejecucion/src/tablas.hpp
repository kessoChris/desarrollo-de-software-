#pragma once
#include "db.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

inline std::vector<std::wstring> wrap(const std::string& text,size_t width) {
    std::wstring s=wide(text);
    std::vector<std::wstring> lines;
    while(s.size()>width) {
        size_t at=s.rfind(L' ',width);
        if(at==std::wstring::npos || at==0) at=width;
        lines.push_back(s.substr(0,at)); s.erase(0,at);
        while(!s.empty() && s.front()==L' ') s.erase(0,1);
    }
    lines.push_back(s); return lines;
}
inline std::string tabla(const std::string& title,const Row& headers,
                         const Rows& rows,const std::vector<size_t>& widths) {
    std::ostringstream out;
    auto border=[&](){out<<'+';for(auto w:widths)out<<std::string(w+2,'-')<<'+';out<<'\n';};
    auto line=[&](const Row& row){
        std::vector<std::vector<std::wstring>> cells;
        size_t height=1;
        for(size_t i=0;i<widths.size();++i){cells.push_back(wrap(row.at(i),widths[i]));height=std::max(height,cells.back().size());}
        for(size_t y=0;y<height;++y){
            out<<'|';
            for(size_t x=0;x<widths.size();++x){
                std::wstring cell=y<cells[x].size()?cells[x][y]:L"";
                out<<' '<<utf8(cell)<<std::string(widths[x]-cell.size(),' ')<<" |";
            }
            out<<'\n';
        }
    };
    out<<title<<'\n';border();line(headers);border();
    for(const auto& row:rows){line(row);border();}
    if(rows.empty()) out<<"Sin registros.\n";
    out<<"Filas: "<<rows.size()<<"\n";
    return out.str();
}
inline std::string leer(const std::string& path){
    std::ifstream f(path,std::ios::binary);
    if(!f) throw std::runtime_error("No se pudo leer "+path);
    std::ostringstream s;s<<f.rdbuf();return s.str();
}
inline void guardar(const std::string& path,const std::string& data){
    std::ofstream f(path,std::ios::binary);
    if(!f || !(f<<data)) throw std::runtime_error("No se pudo guardar "+path);
}
inline std::string literal(const std::string& s){
    std::string q="'";for(char c:s){q+=c;if(c=='\'')q+='\'';}return q+"'";
}
inline void respaldo(Database& db){
    std::ostringstream out;
    out<<"-- Respaldo de rrhh obtenido desde PostgreSQL por ODBC.\n"
          "-- Restaurar en una base NUEVA y vacía con codificación UTF8.\n"
          "BEGIN;\nSET standard_conforming_strings=on;\n"<<leer("database/00_schema.sql")<<'\n';
    const Row names={"centros_trabajo","puestos","empleados","directivos"};
    const Row columns={"centro_id,nombre,ciudad","puesto_id,nombre,descripcion",
        "numero_empleado,nombre,apellido_paterno,apellido_materno,fecha_nacimiento,centro_id,puesto_id,es_directivo",
        "numero_empleado,centro_supervisado_id,apoyo_combustible"};
    for(size_t i=0;i<names.size();++i){
        auto rows=db.query("SELECT "+columns[i]+" FROM rrhh."+names[i]+" ORDER BY 1");
        for(const auto& row:rows){
            out<<"INSERT INTO rrhh."<<names[i]<<" ("<<columns[i]<<") "
               <<((i==1||i==2)?"OVERRIDING SYSTEM VALUE ":"")<<"VALUES (";
            for(size_t j=0;j<row.size();++j){if(j)out<<',';out<<literal(row[j]);}
            out<<");\n";
        }
    }
    for(const auto& p:std::vector<Row>{{"puestos","puesto_id"},{"empleados","numero_empleado"}})
        out<<"SELECT setval(pg_get_serial_sequence('rrhh."<<p[0]<<"','"<<p[1]<<"'), COALESCE(MAX("<<p[1]<<"),1), MAX("<<p[1]<<") IS NOT NULL) FROM rrhh."<<p[0]<<";\n";
    out<<"COMMIT;\n";guardar("database/04_respaldo_a3.sql",out.str());
}
