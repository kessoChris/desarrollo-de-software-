#include <windows.h>
#include <conio.h>
#include <sql.h>
#include <sqlext.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void printDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle) {
    SQLCHAR state[6]{};
    SQLCHAR message[1024]{};
    SQLINTEGER nativeError = 0;
    SQLSMALLINT messageLength = 0;

    for (SQLSMALLINT record = 1;
         SQLGetDiagRecA(handleType, handle, record, state, &nativeError,
                        message, sizeof(message), &messageLength) != SQL_NO_DATA;
         ++record) {
        std::cerr << "ODBC " << state << " (" << nativeError << "): " << message << '\n';
    }
}

bool succeeded(SQLRETURN result) {
    return result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO;
}

struct TableQuery {
    const char* label;
    const char* sql;
};

} // namespace

int main() {
    const char* environmentConnectionString = std::getenv("PERSONAL_UNI_ODBC");
    std::string connectionString =
        environmentConnectionString == nullptr || *environmentConnectionString == '\0'
            ? "Driver={PostgreSQL ODBC Driver(UNICODE)};"
              "Server=127.0.0.1;Port=5432;Database=personal_uni;"
            : environmentConnectionString;
    if (environmentConnectionString == nullptr || *environmentConnectionString == '\0') {
        std::string user;
        std::cout << "Usuario PostgreSQL [postgres]: " << std::flush;
        std::getline(std::cin, user);
        if (user.empty()) user = "postgres";

        std::string password;
        std::cout << "Contraseña PostgreSQL (entrada oculta): " << std::flush;
        for (int key = 0; (key = _getch()) != '\r' && key != '\n';) {
            if (key == '\b') {
                if (!password.empty()) password.pop_back();
            } else if (key >= 32 && key <= 255) {
                password.push_back(static_cast<char>(key));
            }
        }
        std::cout << '\n';
        connectionString += "Uid=" + user + ";Pwd=" + password + ";";
        std::fill(password.begin(), password.end(), '\0');
    }

    SQLHENV environment = SQL_NULL_HENV;
    SQLHDBC connection = SQL_NULL_HDBC;
    SQLHSTMT statement = SQL_NULL_HSTMT;
    SQLRETURN result = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &environment);
    if (!succeeded(result)) {
        std::cerr << "No se pudo crear el entorno ODBC.\n";
        return 1;
    }

    SQLSetEnvAttr(environment, SQL_ATTR_ODBC_VERSION,
                  reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0);

    result = SQLAllocHandle(SQL_HANDLE_DBC, environment, &connection);
    if (!succeeded(result)) {
        std::cerr << "No se pudo crear la conexión ODBC.\n";
        SQLFreeHandle(SQL_HANDLE_ENV, environment);
        return 1;
    }

    std::array<SQLCHAR, 2048> completedConnectionString{};
    SQLSMALLINT completedLength = 0;
    result = SQLDriverConnectA(
        connection, nullptr,
        reinterpret_cast<SQLCHAR*>(connectionString.data()),
        SQL_NTS, completedConnectionString.data(),
        static_cast<SQLSMALLINT>(completedConnectionString.size()),
        &completedLength, SQL_DRIVER_PROMPT);
    if (!succeeded(result)) {
        std::cerr << "No se pudo conectar a personal_uni. Revisa PostgreSQL y "
                     "los datos del cuadro de autenticación ODBC.\n";
        printDiagnostics(SQL_HANDLE_DBC, connection);
        SQLFreeHandle(SQL_HANDLE_DBC, connection);
        SQLFreeHandle(SQL_HANDLE_ENV, environment);
        return 1;
    }

    result = SQLAllocHandle(SQL_HANDLE_STMT, connection, &statement);
    if (!succeeded(result)) {
        std::cerr << "No se pudo crear la instrucción ODBC.\n";
        SQLDisconnect(connection);
        SQLFreeHandle(SQL_HANDLE_DBC, connection);
        SQLFreeHandle(SQL_HANDLE_ENV, environment);
        return 1;
    }

    const std::array<TableQuery, 4> tables{{
        {"CENTROS DE TRABAJO", "SELECT * FROM rrhh.centros_trabajo WHERE 1 = 0"},
        {"PUESTOS", "SELECT * FROM rrhh.puestos WHERE 1 = 0"},
        {"EMPLEADOS", "SELECT * FROM rrhh.empleados WHERE 1 = 0"},
        {"DIRECTIVOS", "SELECT * FROM rrhh.directivos WHERE 1 = 0"},
    }};

    std::cout << "Conexión ODBC a PostgreSQL establecida.\n";
    for (const TableQuery& table : tables) {
        result = SQLExecDirectA(statement,
            reinterpret_cast<SQLCHAR*>(const_cast<char*>(table.sql)), SQL_NTS);
        if (!succeeded(result)) {
            std::cerr << "Error al consultar " << table.label << ":\n";
            printDiagnostics(SQL_HANDLE_STMT, statement);
            SQLFreeHandle(SQL_HANDLE_STMT, statement);
            SQLDisconnect(connection);
            SQLFreeHandle(SQL_HANDLE_DBC, connection);
            SQLFreeHandle(SQL_HANDLE_ENV, environment);
            return 1;
        }

        SQLSMALLINT columnCount = 0;
        SQLNumResultCols(statement, &columnCount);
        std::cout << "\n" << table.label << "\n";
        for (SQLSMALLINT column = 1; column <= columnCount; ++column) {
            std::array<SQLCHAR, 256> columnName{};
            SQLSMALLINT nameLength = 0;
            SQLSMALLINT dataType = 0;
            SQLUINTEGER columnSize = 0;
            SQLSMALLINT decimalDigits = 0;
            SQLSMALLINT nullable = 0;
            result = SQLDescribeColA(statement, column, columnName.data(),
                static_cast<SQLSMALLINT>(columnName.size()), &nameLength,
                &dataType, &columnSize, &decimalDigits, &nullable);
            if (!succeeded(result)) {
                printDiagnostics(SQL_HANDLE_STMT, statement);
                SQLFreeHandle(SQL_HANDLE_STMT, statement);
                SQLDisconnect(connection);
                SQLFreeHandle(SQL_HANDLE_DBC, connection);
                SQLFreeHandle(SQL_HANDLE_ENV, environment);
                return 1;
            }
            if (column > 1) std::cout << " | ";
            std::cout.write(reinterpret_cast<const char*>(columnName.data()), nameLength);
        }
        std::cout << "\nSin registros (solo encabezados).\n";
        SQLFreeStmt(statement, SQL_CLOSE);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, statement);
    SQLDisconnect(connection);
    SQLFreeHandle(SQL_HANDLE_DBC, connection);
    SQLFreeHandle(SQL_HANDLE_ENV, environment);
    return 0;
}
