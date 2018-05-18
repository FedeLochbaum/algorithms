// 558 Wormholes

#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>

using namespace std;
/*
EXPLICACIÓN:
El ejercicio exige saber si es posible viajar al pasado, viajando indefinidamente por diferentes agujeros negros.
 Por lo tanto, podemos decir que, modelando el problema como un grafo podemos utilizar bellman ford para obtener el camino mínimo desde el nodo inicial hacia el resto.
 Una vez realizado el bellman ford, podemos identificar si existe o no un ciclo negativo. Como vimos en clase,
 realizamos una ultima iteración del algoritmo, si se encuentra un camino menor al existente podemos afirmar que existe un ciclo negativo.


Por cada caso, se obtiene el n = cantidad de nodos y m = cantidad de aristas. Se obtienen las m aristas construyendo el grafo (un vector de ternas).
 Se aplica el algoritmo de bellman ford y se verifica si existe un ciclo negativo. Si existe un ciclo negativo se imprime "possible" y finaliza la ejecución.


 Complejidad del algoritmo: O(n . m)

 Time: 0.040
*/

int c, n, m, x, y, t;
int inf = numeric_limits<int>::max();

vector<tuple<long, long, long>> graph;
vector<long> times;

void bellman_ford(){
    for(auto i = 0; i < n; i++) {
        for(auto tuple : graph) {
            times[std::get<1>(tuple)] = min(times[std::get<1>(tuple)], times[std::get<0>(tuple)] + std::get<2>(tuple));
        }
    }
}

void check_negative_cycle() {
    for(auto tuple : graph) {
        if(times[std::get<0>(tuple)] + std::get<2>(tuple) < times[std::get<1>(tuple)]) {
                cout << "possible" << endl;
                return;
        }
    }
    cout << "not possible" << endl;
}

int main() {
    cin >> c;
    while(c > 0) {
        cin >> n;
        cin >> m;

        graph = vector<tuple<long, long, long>>(m);
        times = vector<long>(n, inf);
        times[0] = 0;

        for(auto e = 0; e < m; e++) {
            cin >> x;
            cin >> y;
            cin >> t;

            graph.push_back(make_tuple(x, y, t));
        }

        bellman_ford();
        check_negative_cycle();
        c--;
    }
    return 0;
}
