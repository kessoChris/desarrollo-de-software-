#include <iostream>
#include <string>
#include <cctype>
using namespace std;

class Persona {
private:
    string nombre;
    string apellidoPaterno;
    string apellidoMaterno;
    string fechaNacimiento;

public:
    void capturarDatos() {
        cout << "Ingresa tu nombre: ";
        getline(cin, nombre);

        cout << "Ingresa tu apellido paterno: ";
        getline(cin, apellidoPaterno);

        cout << "Ingresa tu apellido materno (X si no tienes): ";
        getline(cin, apellidoMaterno);

        cout << "Ingresa tu fecha de nacimiento (DD/MM/AAAA): ";
        getline(cin, fechaNacimiento);
    }

    char obtenerVocalInterna() {
        for (int i = 1; i < apellidoPaterno.length(); i++) {
            char letra = toupper(apellidoPaterno[i]);

            if (letra == 'A' || letra == 'E' || letra == 'I' ||
                letra == 'O' || letra == 'U') {
                return letra;
            }
        }
        return 'X';
    }

    string generarRFC() {
        string rfc = "";

        rfc += toupper(apellidoPaterno[0]);
        rfc += obtenerVocalInterna();

        if (apellidoMaterno == "X" || apellidoMaterno.empty()) {
            rfc += 'X';
        } else {
            rfc += toupper(apellidoMaterno[0]);
        }

        rfc += toupper(nombre[0]);

        // Año: últimos dos dígitos
        rfc += fechaNacimiento.substr(8, 2);

        // Mes
        rfc += fechaNacimiento.substr(3, 2);

        // Día
        rfc += fechaNacimiento.substr(0, 2);

        return rfc;
    }
};

int main() {
    Persona empleado;

    cout << "=== CALCULO DE RFC ===" << endl;

    empleado.capturarDatos();

    cout << "\nRFC generado sin homoclave: "
         << empleado.generarRFC() << endl;

    return 0;
}