//1235 - Anti Brute Force Lock

#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <math.h>

using namespace std;

/*
EXPLICACION:
    Idea:
        Leer cada key de las combinaciones que deben ser encontradas.
        Calcular el numero de movimientos que deben hacerse para llegar desde cada una a todas las demás, es decir,
            la cantidad mínima de giros que deben hacerse, hacia arriba o hacia abajo para llegar desde un numero a otro.
            Crear un grafo completo, donde los vértices son 0000 y el resto de las keys aceptadas; y las aristas es el el calculo anteriormente mencionado.
        Utilizar prim para generar el árbol generador mínimo, donde se calcula la arista menos pesada saliente desde un vértice a otro.
        El único caso que se debe contemplar es que, como 0000 no es una posición desbloqueable, siempre se partir del dígito que requiera menos movimientos desde 0000.
        En mi caso para solucionar esto, dentro del algoritmo de prim trato el caso del 0 como especial,
            desasignando el valor mínimo del vector dist para todos los vértices salvo por el de menor peso.

    Nota: Se utiliza la implementación de djsktra pero utilizando vectores.

    Ordenes :
        Construcción del grafo = N^2.
        Prim = Sumatoria desde 0 hasta n de:
                    n (encontrar el siguiente mínimo no marcado)
                                        +
                    sumatoria desde 1 hasta n: comparar y asignar.
                Es decir, Θ(N * 2N) o O(N^2)
        Algoritmo = Θ(3N^2) o O(N^2)

    Time: 0.180
*/

typedef vector<int> vi;
typedef pair<int,int> pii;
typedef vector< pii > vii;

vi  vertexs;
int Cases, N, Current;
vii *graph;
vi dist, parents;
vector<bool> marks;
int inf = numeric_limits<int>::max();

int diff_keys(int number_1, int number_2) {
    int res = 0;

    for (int i = 3; i >= 0; i--) {
        int divisor = pow(10, i);

        int unit1 = number_1 / divisor;
        int unit2 = number_2 / divisor;

        int mi = min(abs(unit1 - unit2),
                    min(abs((unit1 -10) - unit2),
                        abs((unit1 +10) - unit2)));
        res += mi;

        number_1 -= unit1 * divisor;
        number_2 -= unit2 * divisor;
    }
    return res;
}

void print_result() { std::cout << std::accumulate(dist.begin(), dist.end(), 0) << std::endl; }

int min_of_dist() {
    int cur_min = inf;
    int min_index = inf;

    for(int i = 0; i < dist.size(); i++) {
        if(!marks[i] && dist[i] < cur_min) {
            cur_min = dist[i];
            min_index = i;
        }
    }
    return min_index;
}

void prim() {
    marks.assign(vertexs.size(), false);
    dist.assign(vertexs.size(), inf);
    dist[0] = 0;

    for(auto a = 0; a < N+1; a++) {
        int u = min_of_dist();
        marks[u] = true;
        for(auto c : graph[u]) {
            int w = c.first;   // weight
            int v = c.second; // vertex

            if (!marks[v] && w < dist[v]) {
                dist[v] = w;
            }
        }

        if(u == 0) {
            int mi = min_of_dist();
            int val = dist[mi];
            dist = vector<int>(vertexs.size(), inf);
            dist[0] = 0;
            dist[mi] = val;
        }
    }

    print_result();
}

int main() {
    cin >> Cases;

    while(Cases > 0) {
        cin >> N;
        graph =  new vii[N+1];
        vertexs.clear();
        vertexs.push_back(0);
        for(int i = 1; i < N+1; i++) {
            cin >> Current;
            vertexs.push_back(Current);
            for(int a = 0; a < i; a++) {
                int other = vertexs[a];
                int diff = diff_keys(other, Current);
                graph[a].push_back({diff, i});
                graph[i].push_back({diff, a});
            }
        }

        prim();
        Cases--;
    }
    return 0;
}
