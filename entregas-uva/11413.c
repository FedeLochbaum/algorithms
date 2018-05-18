#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

/*
EXPLICACION:

   Se obtiene V como cantidad de recipientes, M como cantidad de contenedores.
   Por cada recipiente se obtiene la capacidad y se guarda la máxima capacidad de todos los recipientes en una variable llamada minimum.
    Esto es ya que posteriormente esta variable sera utilizada como valor mínimo en la busqueda binaria.
    Se mantiene una variable maximum igual a la sumatoria de todas las capacidades y sera utilizado como máximo de la busqueda, análogo al anterior.
    Y finalmente se guarda en un vector de recipientes la capacidad del iesimo recipiente.

   Se implementa una función llamada canFill que denota si para un valor, que representa la capacidad de todos los contenedores, es posible volcar todos los recipientes.
    Es decir, denota true si, designándole a cada contenedor esta capacidad, es posible volcar todos los recipientes utilizando M contenedores.

   Una vez implementada esta función, se prosigue a utilizar busqueda binaria utilizando canFill para denotar si "(maximum + minimum) / 2" es el nuevo máximo o el nuevo mínimo.
   Esta busqueda se hará tantas veces como el logaritmo en base 2 de la sumatoria de todos los recipientes, es decir el logaritmo de maximum.
   Como siempre el valor máximo es aquel en el que si se cumple que todos los contenedores puede ser utilizados y sin dejar sobras, sabemos que el resultado, luego de hacer busqueda binaria quedara en maximum.


   Time: 0.000
*/



vector<int> vessels;
int N, V, M, C, minimum, maximum;

bool canFill(int value) {
    int containers = M;
    int current = value;
    for(int i = 0; i < vessels.size(); i++) {
        if(current >= vessels[i]) {
            current -= vessels[i];
        } else {
            containers --;
            current = value;
            current -= vessels[i];
        }
    }

    if(containers > 0) {
        return true;
    }

    return false;
}

int main() {

    while(cin >> V){ // count of vessels
        minimum = 0;
        maximum = 0;
        cin >> M; // count of containers
        while(V > 0) {
            cin >> C; //capacity of each vessel
            minimum = max(minimum, C);
            maximum += C;
            vessels.push_back(C);
            V--;
        }

        int i = log2(maximum) + 1;

        while(i > 0) {
            int current = (maximum + minimum) / 2;
            if(canFill(current)) {
                maximum = current;
            } else {
                minimum = current;
            }
            i--;
        }


        cout << maximum << endl;
        vessels.clear();
    }

    return 0;
}
