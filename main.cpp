#include <iostream>
#include <string>
using namespace std;

int main() {
    string nombre;
    int edad;

    cout << "Ingresa tu nombre: ";
    getline(cin, nombre);

    cout << "Ingresa tu edad: ";
    cin >> edad;
  
    if (edad >= 18) {
        cout << "Bienvenido " << nombre << ", eres lo suficientemente grande como para ir al bar." << endl;
    } else {
        cout << "Bienvenido " << nombre << ", ..... mejor vete con tu mami pequeño." << endl;
    }

    return 0;
}
