// 103  Stacking Boxes

#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>

using namespace std;

/*
EXPLICACION:
    El ejercicio pide saber que cantidad de cajas y en en que orden pueden ser colocadas de tal forma que dada la secuencia de cajas c_0..c_i, c_i+1. c_i entre en la siguiente, es decir, c_i+1 y así sucesivamente.

    Por cada caso, se obtiene el numero de cajas(K) y la cantidad de dimensiones correspondientes a dichas cajas (N). Se obtienen las K cajas con N dimensiones y se ordenan de menor a mayor (teniendo en cuenta la suma de sus dimensiones).
    Luego, teniendo un vector ordenado de cajas [v0..vi, vi+1] se crea un grafo acíclico donde por cada nodo v0 se agrega una arista a todos aquellos nodos que sean mas grandes que el y que, además, v0 entra en dicha caja.
    Se llama a camino máximo, el cual calcula topological sort e inicializa las distancias de todos los nodos en un vector dist. Luego, como vimos en clase, se reconstruye el camino máximo encontrando, primero, el máximo elemento de dist.
    Luego, se buscan en dist los nodos por los cuales se pasaron para llegar al camino máximo y estos, son guardados en un vector. Una vez calculado todo el recorrido, se imprime el tamaño del vector resultad y cada uno de sus elementos.

    Técnicamente, mi algoritmo no funciona. Estaría fallando en alguna parte, tal vez por algún error de índices. Lamentablemente no pude dedicarle mucho mas que 5 horas a buscar dicho error.
*/

struct Box {
    int number;
    int sum;
    vector<int> measurements;
};

vector<int> to, dist;
vector<bool> marks;
int inf = numeric_limits<int>::max();
int K, N, current, box_number;
vector<Box> boxs;
vector<vector<int>> graph;

bool compare_box (Box i, Box j) { return (i.sum < j.sum); }

bool nests_in(Box i, Box j) {
    for(int e = 0; e < i.measurements.size(); e++) {
        if(i.measurements[e] > j.measurements[e]) {
            return false;
        }
    }
    return true;
}

void dfs(int vertex) {
    marks[vertex] = true;
    for(int i = 0; i < graph[vertex].size(); i++) {
        if(!marks[i]) {
            dfs(i);
        }
    }
    to.insert(to.begin(), vertex);
}

void topological_sort() {
    marks = vector<bool>(box_number, false);
    to = vector<int>();
    for(int i = 0; i < box_number; i++) {
        if(!marks[i]) {
            dfs(i);
        }
    }
}

void longest_path() {
    topological_sort();

    for(int i = 0; i < to.size(); i++) {
        int v = to[i];
        for(int e = 0; e < graph[v].size(); e++) {
            int w = graph[v][e];
            dist[v] = max(dist[v], 1 + dist[w]);
        }
    }
}

void print_result() {
    vector<int> res = vector<int>();
    int pos_max = 0;
    for(int i = 1; i < dist.size(); i++) {
        if(dist[i] > dist[pos_max]) {
            pos_max = i;
        }
    }

    for(int e = 0; e < graph.size(); e++) {
        if(std::find(graph[e].begin(), graph[e].end(), pos_max) != graph[e].end()) {
            if(dist[e] == dist[pos_max] - 1) {
                res.insert(res.begin(), e);
            }

            if(dist[pos_max] == 0) { break; }
        }
    }

    cout << res.size() << endl;
    for(int a = 0; a < res.size(); a++) {
        cout << res[a]+1 << " ";
    }
    cout << endl;
}

void resolve() {
    graph =  vector<vector<int>>(box_number);
    dist = vector<int>(box_number, 0);

    for(int i = 0; i < boxs.size() -1; i++) {
        Box current = boxs[i];

         for(int e = i+1; e < boxs.size(); e++) {
             Box next = boxs[e];
            if(nests_in(current, next)) {
                graph[current.number-1].push_back(next.number-1);
            }
        }
    }

    longest_path();

    print_result();
    graph.clear();
    boxs.clear();
}


int main(){

    while(cin >> K) {
        box_number = K;
        cin >> N;
        while(K > 0) {
            Box box = { boxs.size() +1, 0, vector<int>(N)};
            for(int i = 0; i < N; i++) {
                cin >> current;
                box.sum += current;
                box.measurements.push_back(current);
            }
            std::sort (box.measurements.begin(), box.measurements.end());
            boxs.push_back(box);
            K--;
        }
        std::sort (boxs.begin(), boxs.end(), compare_box);

        resolve();
    }


    return 0;
}
