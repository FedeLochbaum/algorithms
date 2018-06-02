#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>

using namespace std;

typedef vector<int> vi;
typedef vector<vi> mi;

//////////////////////////////////////////////// Frequency /////////////////////////////////////////////////
struct Frequency {
    int cost_of_use;
    int number_id;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////// Global variables ////////////////////////////////////////////
int current, i, j, cost;
int T; // numero de frecuecncias
int N; // numero de antenas disponibles
int M; // numero de conflictos posibbles por asignar con la misma frecuencia
mi graph;
vector<Frequency> f;  // indica el costo de usar la frecuencia i
mi cost_conflict_matrix; // costo de colisiones de frecuencias para el par de antenas i, k

////////////////////////////////////////////// Stop Criteria ///////////////////////////////////////////////
int maximum_iterations = 1000;
int current_iteration;
int maximum_time = 300; // 5 minutes
time_t start_time;

///////////////////////////////////////////// Order function //////////////////////////////////////////////
bool order_func (Frequency i, Frequency j) { return (i.cost_of_use < j.cost_of_use); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////// Solution /////////////////////////////////////////////////
struct Solution {
    vi y; // 0 o 1 si la frecuencia con id i esta usada
    vi assings; // assings[i] indica la frecuencia asignada al vertice i.

    Solution();
    int functional();
    int loss_for_collision();
    int cost_of_frequencies();
    void show_solution();
    void update_frequencies();
};

Solution::Solution() {
    y = vi(T,0);
    assings = vi(N, -1);
}

void Solution::update_frequencies() {
    //TODO: optimizar
    y = vi(y.size(), 0);
    for (int assing : assings) { y[assing] = 1; }
}

int Solution::loss_for_collision(){
    auto copy_matrix = cost_conflict_matrix;
    int res = 0;

    for(int i = 0; i < copy_matrix.size(); i ++) {
        for(int k = 0; k < copy_matrix[i].size(); k++) {
            if(assings[i] == assings[k]) {
                res += copy_matrix[i][k];
                copy_matrix[i][k] = 0;
                copy_matrix[k][i] = 0;
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
    for(auto frec_id : assings) { cout << frec_id << endl; }
    cout << "Cost: " << functional() << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

Solution greedy_solution(int seed_vertex) {
    cout << "Seed vertex = " << seed_vertex << endl;

    Solution sol = Solution();

    Frequency freq{}; // Sacar inicializacion luego
    freq = f[0]; // Esta ordenado
    sol.y[freq.number_id] = 1;
    sol.assings[seed_vertex] = freq.number_id;

    for(int i = 0; i < graph.size(); i++) {
        if(sol.assings[i] == -1) { // Si el vertice actual no tiene frecuencia asignada
            vector<Frequency> possible_frequencies = f;
            for(int e = 0; e < graph[i].size(); e++) {
                auto current_e = graph[i][e];
                if(sol.assings[current_e] != -1) { // Si el vertice lindante ya posee un color

                    // Se busca la frecuencia con id = sol.assings[sol.graph[i][e]] y se elimina
                    for(int j = 0; j < possible_frequencies.size(); j++) {
                        if(possible_frequencies[j].number_id == sol.assings[current_e]) {
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

Solution local_search(Solution sol) {
    for(int i = 0; i < N; i ++) {
        for(int f = 0; f < T; f++) {
            int old_freq = sol.assings[i];
            int old_cost = sol.functional();
            if(old_freq != f) {
                sol.assings[i] = f;
                sol.update_frequencies();

                if(old_cost < sol.functional()) {
                    sol.assings[i] = old_freq;
                    sol.y[old_freq] = 1;
                }
            }
        }
    }

    return sol;
}

bool stop_criteria() { return current_iteration >= maximum_iterations || time(0) >= start_time + maximum_time; }

Solution grasp() {
    start_time = time(0);
    current_iteration = 0;

    Solution global_solution = greedy_solution(0);

    while(!stop_criteria()) {
        srand(static_cast<unsigned int>(time(nullptr)));
        int seed_vertex = (rand() + current_iteration) % N;

        Solution current_solution = local_search(greedy_solution(seed_vertex));
        if(current_solution.functional() < global_solution.functional()) {
            global_solution = current_solution;
        }
        current_iteration++;
    }

    return global_solution;
}

int main() {
    cin >> N;
    cin >> T;
    cin >> M;

    f = vector<Frequency>();
    cost_conflict_matrix = vector<vi>(N, vector<int>(N, 0));

    for(auto i = 0; i < T; i++) {
        cin >> current;
        f.push_back({current, i}); // {current, i} :: Frequency
    }

    // Se ordena f por costo de uso
    sort (f.begin(), f.end(), order_func);

    graph = vector<vi>(N, vi());

    for(auto e = 0; e < M; e++) {
        cin >> i;
        cin >> j;
        cin >> cost;

        // Indices desde 0 hasta N-1
        i--;
        j--;

        cost_conflict_matrix[i][j] = cost;
        cost_conflict_matrix[j][i] = cost;

        graph[i].push_back(j);
        graph[j].push_back(i);
    }

    grasp().show_solution();

    return 0;
}