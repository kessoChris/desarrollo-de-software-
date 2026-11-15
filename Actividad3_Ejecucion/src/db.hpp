#pragma once
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <vector>
#include <stdexcept>

using Row = std::vector<std::string>;
using Rows = std::vector<Row>;
inline std::wstring wide(const std::string& s) {
    int n=MultiByteToWideChar(CP_UTF8,0,s.data(),(int)s.size(),nullptr,0);
    std::wstring r(n,0); MultiByteToWideChar(CP_UTF8,0,s.data(),(int)s.size(),r.data(),n); return r;
}
inline std::string utf8(const std::wstring& s) {
    int n=WideCharToMultiByte(CP_UTF8,0,s.data(),(int)s.size(),nullptr,0,nullptr,nullptr);
    std::string r(n,0); WideCharToMultiByte(CP_UTF8,0,s.data(),(int)s.size(),r.data(),n,nullptr,nullptr); return r;
}
inline std::string odbcValue(std::string s) {
    std::string r="{"; for(char c:s) { r+=c; if(c=='}') r+='}'; } return r+'}';
}
class Database {
    SQLHENV env=SQL_NULL_HENV;
    SQLHDBC dbc=SQL_NULL_HDBC;
    void cleanup() noexcept {
        if(dbc) { SQLDisconnect(dbc); SQLFreeHandle(SQL_HANDLE_DBC,dbc); dbc=SQL_NULL_HDBC; }
        if(env) { SQLFreeHandle(SQL_HANDLE_ENV,env); env=SQL_NULL_HENV; }
    }
    static void check(SQLRETURN rc, SQLSMALLINT type, SQLHANDLE h) {
        if(SQL_SUCCEEDED(rc)) return;
        SQLWCHAR state[6]{},message[1024]{}; SQLINTEGER native=0; SQLSMALLINT len=0;
        SQLGetDiagRecW(type,h,1,state,&native,message,1024,&len);
        throw std::runtime_error(utf8((wchar_t*)state)+": "+utf8((wchar_t*)message));
    }
public:
    Database(const std::string& user, const std::string& password) {
        try {
            if(!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env)))
                throw std::runtime_error("No se pudo crear el entorno ODBC");
            check(SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,0),SQL_HANDLE_ENV,env);
            check(SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc),SQL_HANDLE_ENV,env);
            auto conn=wide("Driver={PostgreSQL ODBC Driver(UNICODE)};Server=127.0.0.1;Port=5432;Database=personal_uni;Uid="+user+";Pwd="+password+";");
            SQLRETURN rc=SQLDriverConnectW(dbc,nullptr,(SQLWCHAR*)conn.data(),SQL_NTS,nullptr,0,nullptr,SQL_DRIVER_NOPROMPT);
            SecureZeroMemory(conn.data(),conn.size()*sizeof(wchar_t));
            check(rc,SQL_HANDLE_DBC,dbc);
        } catch(...) { cleanup(); throw; }
    }
    Database(const Database&)=delete;
    Database& operator=(const Database&)=delete;
    ~Database() { cleanup(); }
    Rows query(const std::string& sql) {
        SQLHSTMT stmt=SQL_NULL_HSTMT;
        check(SQLAllocHandle(SQL_HANDLE_STMT,dbc,&stmt),SQL_HANDLE_DBC,dbc);
        try {
            auto statement=wide(sql);
            check(SQLExecDirectW(stmt,(SQLWCHAR*)statement.data(),SQL_NTS),SQL_HANDLE_STMT,stmt);
            SQLSMALLINT cols=0;
            check(SQLNumResultCols(stmt,&cols),SQL_HANDLE_STMT,stmt);
            Rows rows;
            while(cols>0) {
                SQLRETURN rc=SQLFetch(stmt);
                if(rc==SQL_NO_DATA) break;
                check(rc,SQL_HANDLE_STMT,stmt);
                Row row;
                for(SQLUSMALLINT i=1;i<=cols;++i) {
                    std::wstring value;
                    do {
                        SQLWCHAR buffer[512]{}; SQLLEN indicator=0;
                        rc=SQLGetData(stmt,i,SQL_C_WCHAR,buffer,sizeof(buffer),&indicator);
                        if(rc==SQL_NO_DATA) break;
                        check(rc,SQL_HANDLE_STMT,stmt);
                        if(indicator==SQL_NULL_DATA) break;
                        value+=(wchar_t*)buffer;
                    } while(rc==SQL_SUCCESS_WITH_INFO);
                    row.push_back(utf8(value));
                }
                rows.push_back(std::move(row));
            }
            SQLFreeHandle(SQL_HANDLE_STMT,stmt); return rows;
        } catch(...) { SQLFreeHandle(SQL_HANDLE_STMT,stmt); throw; }
    }
};

