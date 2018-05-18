#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

/*
EXPLICACIÓN:
    Surgió como una posible mejora de la implementación anterior.
    La idea era simplificar la cantidad de chequeos sobre un numero demasiado grande.
    Ya que se busca dividir de la forma mas equitativa entre dos personas, hacemos foco solo en uno solo de los lados (la mitad) y luego restamos para saber cual sera la diferencia entre este numero y el total.

    Entonces, la matriz es inicializada con la misma cantidad de monedas, pero solo con (maxM / 2), dado que solo se buscara rellenar (maxM / 2) con las monedas existentes.

    Se realiza un knapsack sencillo (como siempre utilizando la matriz de memoizacion) que devuelve el mínimo monto restante de intentar rellenar (maxM / 2) y se lo resta a la mitad, quedando en val el valor de las monedas repartidas a (maxM / 2).

    Entonces sabemos que (maxM - val) es el monto asignado a la otra persona, por lo tanto devolvemos la diferencia entre este ultimo y el monto de la primer persona.

    Lamentablemente, esta solución no mejoro en tiempo a la anterior pero la forma de resolver el problema fue mucho mas sencillo.

    Time: 0.050
*/

int N, M, money, maxM, val;
vector<int> elems;
vector<vector<int>> solutions;

int knapsack(int i, int amount) {
    if(i == elems.size() || amount == 0) { return amount; }

    int current = elems[i];

    if(solutions[i][amount] == -2) {
        if(amount - current >= 0) {
            solutions[i][amount] = min(knapsack(i+1, amount - current), knapsack(i+1, amount));
        } else { solutions[i][amount] = knapsack(i+1, amount); }
    }

    return solutions[i][amount];
}


int main() {
    cin >> N;

    while(N != 0){
        cin >> M;
        maxM = 0;
        while(M > 0) {
            cin >> money;
            maxM += money;
            elems.push_back(money);
            M--;
        }
        solutions = vector<vector<int>>(elems.size(), vector<int>((maxM / 2)+1, -2));

        val = (maxM / 2) - knapsack(0, (maxM / 2));

        cout << abs((maxM - val) - val) << endl;

        N--;
        elems.clear();
    }

    return 0;
}

