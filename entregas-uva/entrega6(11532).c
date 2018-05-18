#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <set>
using namespace std;

/*
EXPLICACION:
    Lamentablemente, por diversas cuestiones, no logre completar la implementación. Indague diferentes opciones y la mas concreta que se me ocurrió es la implementada en este archivo.
    NO funciona en todos los casos, algunos dan diferentes números, al parecer, la decisión  "greedy" que tomo no es tan buena o no me garantiza todo el tiempo que se mantiene tanto la propiedad de mínimo como la de cantidad de adyacentes.

    Mi implementación: Obtengo el P Y Q por cada caso, P representando la cantidad de unos y Q la cantidad de ceros. Recreo el binario en un vector,
    comenzando siempre del numero mas chico representable con (P+Q) bits.
    Es decir, en caso de P = 3 y Q = 2, el binario mas chico y que se recrea es = 0 0 1 1 1.
    Luego, recorro la cantidad de ceros, comenzando desde el primer cero hacia el ultimo, es decir, los últimos Q elementos del vector.
    En cada uno de estos ceros, reviso si es posible swapear, en caso de ser posible swapea ambos valores y continua con el siguiente cero. Un swap se podrá hacer cuando exista esta secuencia 1 1 1, entonces el 1 que sera reemplazado sera el del medio.
    Finalmente, se traduce dicho vector a un numero long long, recorriendo el vector y sumándole las potencias de dos correspondientes en caso de que el iesimo elemento sea = 1.


    Critica: Considero que mi condición de swapeo no es correcta y que tal vez debería haberla realizado sobre otro subconjunto de bits. Asumo que corrigiendo la condición de swap la implementación podría haber funcionado.
    Como no funciono en UVA no opte por demostrar que la propiedad greedy elegida sea óptima (por que no lo es).
*/

int C, P, Q, current_p;
vector<int> binary;

void setBinary() {
    current_p = P;
    for(int i = 0; i < (P+Q); i++) {
        if(current_p > 0) {
            binary.push_back(1);
            current_p--;
        } else {
            binary.push_back(0);
        }
    }
}

int toNumber(){
    int number = 0;
    for(int i = 0; i < binary.size(); i++){
        if(binary.at(i) == 1) {
            number += pow(2,i);
        }
    }
    return number;
}

int resolve() {
    setBinary();

    for(int i = P; i < binary.size(); i++) {
        if(binary.at(i) == 0) {
            for(int e = 1; e < binary.size(); e++) {
                if(binary.at(e) == 1 && binary.at(e-1) == 1 && binary.at(e+1) == 1) {
                    binary[e] = 0;
                    binary[i] = 1;
                    break;
                }
            }
        }
    }

    return toNumber();
}


int main() {
    cin >> C;

    while(C != 0) {
        cin >> P;
        cin >> Q;

        cout << resolve() << endl;
        binary.clear();
        C--;
    }

    return 0;
}
