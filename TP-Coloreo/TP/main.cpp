#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <ctime>
#include <algorithm>

using namespace std;

typedef pair<int,int> pii;
typedef vector<int> vi;
typedef vector<vi> mi;

//////////////////////////////////////////////// Frequency /////////////////////////////////////////////////
struct Frequency {
    int cost_of_use;
    int number_id;
    Frequency(int cost, int id);
};

Frequency::Frequency(int cost, int id) {
    cost_of_use = cost;
    number_id = id;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////// Global variables ////////////////////////////////////////////
int current, i, j, cost;
int T; // numero de frecuecncias
int N; // numero de antenas disponibles
int M; // numero de conflictos posibbles por asignar con la misma frecuencia
mi general_graph;

vector<Frequency> f;  // indica el costo de usar la frecuencia i
mi cost_conflict_matrix; // costo de colisiones de frecuencias para el par de antenas i, k


bool order_func (Frequency i, Frequency j) { return (i.cost_of_use < j.cost_of_use); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////// Solution /////////////////////////////////////////////////
struct Solution {
    vi y; // 0 o 1 si la frecuencia con id i esta usada
    vi assings; // assings[i] indica la frecuencia asignada al vertice i.
    mi graph;

    Solution();
    int functional();
    int loss_for_collision();
    int cost_of_frequencies();
    void show_solution();
};

Solution::Solution() {
    y = vi(T,0);
    assings = vi(N, -1);
    graph = general_graph;
}

int Solution::loss_for_collision(){
    int res = 0;
    for(int i = 0; i < cost_conflict_matrix.size(); i ++) {
        for(int k = 0; k < cost_conflict_matrix[i].size(); k++) {
            if(assings[i] == assings[k]) {
                res += cost_conflict_matrix[i][k];
            }
        }
    }
    return res;
}

int Solution::cost_of_frequencies() {
    int res = 0;
    for(int i = 0; i < T; i++) {
        if(y[i] == 1) {
            res += f[i].cost_of_use;
        }
    }

    return res;
}

int Solution::functional() {
    return cost_of_frequencies() + loss_for_collision();
}

void Solution::show_solution() { // por ahora no escribe en files
    for(auto i = 0; i < N; i++) {
        cout << assings[i] << endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

Solution greedy_solution(int seed_vertex) {
    cout << "Seed vertex = " << seed_vertex << endl;

    Solution sol = Solution();

    Frequency freq = Frequency(0, 0); // Sacar inicializacion luego
    freq = f[0]; // Esta ordenado
    sol.y[freq.number_id] = 1;
    sol.assings[seed_vertex] = freq.number_id;

    for(int i = 0; i < sol.graph.size(); i++) {
        if(i != seed_vertex){ // Si el vertice actual es distinto al que ya esta marcado.
            vector<Frequency> possible_frequencies = f;
            for(int e = 0; e < sol.graph[i].size(); e++) {
                if(sol.assings[sol.graph[i][e]] != -1) { // Si el vertice lindante ya posee un color

                    // Se busca la frecuencia con id = sol.assings[sol.graph[i][e]] y se elimina si no fue eliminada anteriormente
                    for(int j = 0; j < possible_frequencies.size(); j++) {
                        if(possible_frequencies[j].number_id == sol.assings[sol.graph[i][e]]) {
                            possible_frequencies.erase(possible_frequencies.begin() + j);
                        }
                    }
                }
            }

            if(possible_frequencies.empty()) { // Si no puede asignar ninguna frecuencia sin repetir
                freq = f[0];
            } else { // Puede asignar alguna frecuencia de possible_frequencies
                sort (possible_frequencies.begin(), possible_frequencies.end(), order_func);
                freq = possible_frequencies[0];
                sol.y[freq.number_id] = 1;
            }

            sol.assings[i] = freq.number_id;
        }
    }

    return sol;
}


Solution grasp() {
    srand(static_cast<unsigned int>(time(nullptr)));

    int seed_vertex = rand() % N;
    Solution global_solution = greedy_solution(seed_vertex);

//    while(!stop_criteria()) {
//        Solution current_solution = local_search(greedy_solution());
//        if(current_solution.functional() < global_solution.functional()) {
//            global_solution = current_solution;
//        }
//    }
    return global_solution;
}

int main() {
    cin >> N;
    cin >> T;
    cin >> M;

    f = vector<Frequency>(T);
    cost_conflict_matrix = vector<vi>(N, vector<int>(N));

    for(auto i = 0; i < T; i++) {
        cin >> current;
        Frequency freq = Frequency(current, i);
        f.push_back(freq);
    }

    // Se ordena f por costo de uso
    sort (f.begin(), f.end(), order_func);

    general_graph = vector<vi>(N, vi());

    for(auto e = 0; e < M; e++) {
        cin >> i;
        cin >> j;
        cin >> cost;

        // Indices desde 0 hasta N-1
        i--;
        j--;

        cost_conflict_matrix[i][j] = cost;
        cost_conflict_matrix[j][i] = cost;

        general_graph[i].push_back(j);
        general_graph[j].push_back(i);
    }

    grasp().show_solution();

    return 0;
}