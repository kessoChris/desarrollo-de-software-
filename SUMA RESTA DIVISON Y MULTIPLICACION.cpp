#include <iostream>
using namespace std;

int main() {
    double numero1, numero2;

    cout << "Escribe el primer numero que deseas: ";
    cin >> numero1;

    cout << "Escribe el segundo numero que desea: ";
    cin >> numero2;

    cout << "La suma es: " << numero1 + numero2 << endl;
    cout << "La resta es: " << numero1 - numero2 << endl;
    cout << "La multiplicacion es: " << numero1 * numero2 << endl;

    if (numero2 != 0) {
        cout << "La division es: " << numero1 / numero2 << endl;
    } else {
        cout << "La division no se puede realizar entre cero." << endl;
    }

    return 0;
}