#include <vector>
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

vi top_sort;
vector<bool> marks;
int current, i, j, cost;
int T; // numero de frecuecncias
int N; // numero de antenas disponibles
int M; // numero de conflictos posibbles por asignar con la misma frecuencia
mi graph; // lista de adyacencias
mi complete_graph; // grafo completo
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

void dfs(int vertice) {
    marks[vertice] = true;
    for (int &i : graph[vertice]) {
        if(!marks[i]) {
            dfs(i);
        }
    }

    top_sort.push_back(vertice);
}

void topological_sort() {
    marks = vector<bool>(N, false);
    top_sort = vi();
    for(int i = 0; i < N; i++) {
        if(!marks[i]) {
            dfs(i);
        }
    }
}

Solution greedy_solution() {
    Solution sol = Solution();
    topological_sort();
    for (auto vertex : top_sort){
        Frequency last_freq_used{};
        for(Frequency freq : f) {
            if(sol.assings[vertex] == -1) { // si no tiene ninguna frecuencia asignada.
                sol.assings[vertex] = freq.number_id;
                sol.y[freq.number_id] = 1;
                last_freq_used = freq;
            } else {
                int current_cost = sol.functional();
                // sol.y[freq.number_id] = 1; // update ya lo hace
                sol.assings[vertex] = freq.number_id;
                sol.update_frequencies();

                if(current_cost < sol.functional()) { // si no mejoro la anterior
                    sol.assings[vertex] = last_freq_used.number_id;
                    sol.update_frequencies();
                } else { // si si mejoro el costo
                    last_freq_used = freq;
                }
            }
        }
    }

    return sol;
}


Solution grasp_greedy_solution() {
    //TODO
    return greedy_solution();
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

    Solution global_solution = greedy_solution();

    while(!stop_criteria()) {
        Solution current_solution = local_search(grasp_greedy_solution());
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
    complete_graph = vector<vi>(N, vi(N, 0));

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

        complete_graph[i][j] = 1;
        complete_graph[j][i] = 1;
    }

    grasp().show_solution();

    return 0;
}