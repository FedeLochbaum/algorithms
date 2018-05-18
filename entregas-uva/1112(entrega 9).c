// 1112  Mice and Maze

#include <utility>
#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <queue>

using namespace std;

/*
EXPLICACION:
    Básicamente, el ejercicio pide saber cuantos nodos pueden llegar al nodo salida con un tiempo de recorrido menor o igual a T.

    Por cada caso, se obtiene la cantidad de nodos (N), el nodo salida (E), el tiempo máximo que se puede tardar hasta llegar a la salida (T) y la cantidad de aristas.
    Por cada arista, se agrega al grafo los nodos lindantes a B, por ejemplo, a partir de A puede llegarse a el(B) con tiempo Ti.

    Luego, se utiliza dijkstra para obtener el camino mínimo de dicho grafo y crear un vector de las distancias.
    Finalmente se itera el vector de distancia contando que nodos fueron aquellos que lograron salir con un Tiempo (dist[nodo actual]) menor o igual al T inicial.
    El resultado del conteo representa todos los ratones que pueden salir en tiempo <= T.

    Time: 0.000
*/

typedef vector<int> vi;
typedef pair<int,int> pii;
typedef vector< pii > vii;

int Cases, N, E, T, M, A, B, Ti;
vii *graph;
vi dist;
int inf = numeric_limits<int>::max();

void dijkstra() {
    priority_queue<pii, vector<pii>, greater<pii> > p_queue;
    dist.assign(N, inf);
    dist[E] = 0;
    p_queue.push({0, E});
    while(!p_queue.empty()){
        int u = p_queue.top().second;
        p_queue.pop();
        for(auto &c : graph[u]){
            int v = c.first;
            int w = c.second;
            if(dist[v] > dist[u] + w){
                dist[v] = dist[u] + w;
                p_queue.push({dist[v], v});
            }
        }
    }
}

void print_result() {
    int count = 0;

    for(int i = 0; i < dist.size(); i++) {
        if(i != E) {
            if(dist[i] <= T){
				count++;
			}
        } else { count++; }
    }

    cout << count << endl;
  	if(Cases != 1) { cout << endl; }
}


int main() {
    cin >> Cases;
    while(Cases > 0) {
        cin >> N >> E >> T >> M;

        E--;
        graph =  new vii[N];

        for(int i = 0; i < M; i++) {
            cin >> A >> B >> Ti;
            graph[B-1].push_back({A-1, Ti});
        }

        dijkstra();
        print_result();
        Cases--;
    }

    return 0;
}
