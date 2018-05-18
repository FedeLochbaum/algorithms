//775 - Hamiltonian Cycle

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

/*
EXPLICACIÓN:
    La idea del ejercicio era exactamente lo visto en clase, dado una cantidad de vértices (numerados del 1 al N) y las aristas que conectan dichos vértices.
    Devolver un ciclo hamiltoneano en caso de que dicho grafo sea hamiltoneano.

    El algoritmo utilizado es aquel charlado en la clase, el cual, dado cualquier permutación de las aristas (se utiliza 1, ...., N, 1), encontrar un I tal que (I,I+1) no pertenece a las aristas del grafo.
    Luego, encontrar un J tal que: Existe la arista (I, J) e (I+1, J+1). Una vez encontrada, invertir el orden en el vector cycle actual. Repetir este proceso N veces (una por vértice).

    Actualmente, este algoritmo, no devuelve las mismas soluciones que hace el debug de uhunt. Pero es cierto que si devuelve un ciclo hamiltoneano para un grafo.
    Por este motivo, no pude corroborar la exactitud con los casos del sitio.

    Orden de complejidad: O(N^2).
*/

typedef vector<int> vi;
typedef pair<int,int> pii;
typedef vector< pii > vii;
typedef vector< bool > vb;

int N, first, second;
vector<vb> graph;
vi cycle;
char char_first;

bool is_hamiltonian_cycle() {
    for(int e = 0; e < cycle.size()-1; e++) {
        auto current = cycle[e];
        auto next = cycle[e+1];
        if(!graph[current][next]) {
            return false;
        }
    }

    return true;
}

void print_result(){
    if(is_hamiltonian_cycle()) {
        for(int i = 0; i < cycle.size()-1; i++) { cout << cycle[i] << " "; }
        cout << cycle[cycle.size()-1];
        cout << endl;
    } else { cout << 'N' << endl; }
}

void hamiltonian_cycle() {
    cycle.clear();
    for(int i = 1; i < N; i++) {
        cycle.push_back(i);
    }
    cycle.push_back(1);

    for (int i = 1; i < N-1; i++) {
        if (!graph[cycle[i]][cycle[i+1]]) {
            for (int j = 0; j < N-1; j++) {
                if (j < i-1 || j > i+1) {
                    if (graph[cycle[i]][cycle[j]] && graph[cycle[i+1]][cycle[j+1]]) {
                        reverse(cycle.begin() + min(i+1, j+1), cycle.begin() + max(i+1, j+1));
                    }
                }
            }
        }
    }

    print_result();
}


int main(){
    while(cin >> N) {
        N++;
        graph = vector<vb>(N, vector<bool>(N, false));
        while(cin >> char_first && char_first != '%' ) {
            first = char_first -48;
            cin >> second;
            graph[first][second] = true;
            graph[second][first] = true;
        }

        hamiltonian_cycle();
    }

    return 0;
}
