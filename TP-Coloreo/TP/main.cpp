#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <queue>
#include <random>

using namespace std;

typedef pair<int, int> pi;
typedef vector<pi> vpi;
typedef vector<int> vi;
typedef vector<vi> mi;

//////////////////////////////////////////////// Frequency /////////////////////////////////////////////////
struct Frequency {
    int cost_of_use;
    int number_id;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////// Global variables ////////////////////////////////////////////
int inf = numeric_limits<int>::max();
vi top_sort;
vector<bool> marks;
int current, i, j, cost;
int T; // Numero de frecuecncias
int N; // Numero de antenas disponibles
int M; // Numero de conflictos posibbles por asignar con la misma frecuencia
mi graph; // Lista de adyacencias
vector<Frequency> f; // Vector de Frequency ordenado por costo
vi cost_of_freqs; // Indica el costo de usar la frecuencia i
mi cost_conflict_matrix; // Costo de colisiones de frecuencias para el par de antenas i, k


///////////////////////////////////////////// Global variables for greedy randomized construction //////////
vpi C; // Vector de pares de todas las posibles asignacinoes
int K = 4; // Cantidad de elementos maximo de CRL
unsigned int seed = 23;
mt19937 mt(seed);
////////////////////////////////////////////// Reactive GRASP //////////////////////////////////////////////
int count_of_iteration_for_recalculate = 20;

vector<double> possibles_a = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
vector<double> quality_of_average_costs = vector<double>(possibles_a.size());
vector<double> probabilities = vector<double>(possibles_a.size(), 1.0/ possibles_a.size()); // Todos arrancan con probabilidad 1 / m donde m es la cantidad de elementos (arranca equiprobable)
vector<int> cost_averages = vi(possibles_a.size()); // El costo promedio de soluciones encontradas para cada a_i
vector<vi> all_cost_for_each_a = vector<vi>(possibles_a.size()); // Todos los costos guardados, usados para calcular cost_averages
////////////////////////////////////////////// Stop Criteria ///////////////////////////////////////////////
int maximum_iterations = 10000;
int current_iteration;
int maximum_time = 300; // 5 Minutos
time_t start_time;

///////////////////////////////////////////// Order function //////////////////////////////////////////////
bool order_func (Frequency i, Frequency j) { return (i.cost_of_use < j.cost_of_use); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////// Solution /////////////////////////////////////////////////
struct Greater_compare_assign{ bool operator()(const pair<int, pi> &t1, const pair<int, pi> &t2) const { return t1.first < t2.first; } };

struct Less_compare_assign{ bool operator()(const pair<int, pi> &t1, const pair<int, pi> &t2) const { return t1.first > t2.first; } };

struct Solution {
    vi used_frequencies; // used_frequencies[i] Indica cuantas antenas usan la frecuencia i
    vi assings; // assings[i] Indica la frecuencia asignada al vertice i.
    int cost;

    Solution();
    void apply_assign(pi pair);
    void show_solution();
    bool is_complete();
    int cost_of_assign(pi i);
    priority_queue<pair<int, pi>, vector<pair<int, pi>>, Greater_compare_assign> get_RCL(vpi &possibles, double &a);
    inline int cost_of_current_assign(int &vertex, int &new_assign);
    inline int cost_of_new_assign(int &vertex, int &new_assign);
};

Solution::Solution() {
    used_frequencies = vi(T,0);
    assings = vi(N, -1);
    cost = 0;
}

void Solution::apply_assign(pi pair) {
    cost += cost_of_assign(pair);
    int old_assign = assings[pair.first];
    if (old_assign != -1) used_frequencies[ old_assign ] -= 1;
    assings[pair.first] = pair.second;
    used_frequencies[pair.second]+=1;
}

bool Solution::is_complete(){
    for(auto &assing : assings) {
        if(assing == -1) { return false; }
    }
    return true;
}

priority_queue<pair<int, pi>, vector<pair<int, pi>>, Greater_compare_assign > Solution::get_RCL(vpi &possibles, double &a) {
    priority_queue<pair<int, pi>, vector<pair<int, pi>>, Greater_compare_assign > p_queue;

    int c_max = 0;
    int c_min = inf;

    if(possibles.size() <= K) {
        for(auto &pair : possibles) {
            int current_cost = cost_of_assign(pair);
            c_max = max(c_max, current_cost);
            c_min = min(c_min, current_cost);
            p_queue.push({current_cost, pair});
        }
    } else {
        for(int i = 0; i < K; i++) {
            int current_cost = cost_of_assign(possibles[i]);
            c_max = max(c_max, current_cost);
            c_min = min(c_min, current_cost);
            p_queue.push({current_cost, possibles[i]});
        }

        for(auto k = K; k < possibles.size(); k++) {
            int current_cost = cost_of_assign(possibles[k]);
            c_max = max(c_max, current_cost);
            c_min = min(c_min, current_cost);
            if(current_cost < p_queue.top().first) {
                p_queue.pop();
                p_queue.push({current_cost, possibles[k]});
            }
        }
    }

    auto upper_bound = static_cast<int>(c_min + a * (c_max - c_min)); // Cota superior variable segun a

    while(p_queue.top().first > upper_bound) { p_queue.pop(); } // Elimina todas las asignaciones que superan la cota superior

    return p_queue;
}

inline int Solution::cost_of_current_assign(int &vertex, int &new_assign) {
    int current_cost = 0;
    int  current_assign = assings[vertex];

    if (current_assign == -1) { return 0; } // Si la anterior asignacion era -1, no generaba ningun costo tal asignacion
    if (used_frequencies[current_assign] == 1) current_cost += cost_of_freqs[ current_assign ]; // si la frecuencia solo estaba usada por este vertice
    else
        for(auto &u : graph[vertex]) { if (assings[u] == current_assign) { current_cost += cost_conflict_matrix[u][vertex]; } }

    return current_cost;
}

inline int Solution::cost_of_new_assign(int &vertex, int &new_assign) {
    int current_cost = 0;

    if (new_assign == -1) { return 0; }

    if(used_frequencies[new_assign] == 0) current_cost += cost_of_freqs[new_assign]; // Si la frecuencia no esta usada
    else
        for(auto &u : graph[vertex]) { if(assings[u] == new_assign) { current_cost += cost_conflict_matrix[u][vertex]; } }

    return current_cost;
}

int Solution::cost_of_assign(pi i) {
    // Al costo de la nueva asignacion le resto el costo de la vieja asignacion. Por lo tanto, si es negativo, el la solucion es mejor
    return cost_of_new_assign(i.first, i.second) - cost_of_current_assign(i.first, i.second);
}

void Solution::show_solution() { // Por ahora no escribe en files
    for(auto &frec_id : assings) { cout << frec_id << endl; }
    cout << "Cost: " << cost << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void dfs(int &vertice) {
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

void initialize_all_possible_assignments() { // Inicializa C
    topological_sort();

    C = vpi(); // Siempre va a tener dimension N*T

    for (auto vertex : top_sort){
        for (int i = 0; i < f.size(); i++) {
            C.push_back({vertex, i});
        }
    }
}

Solution greedy_solution() {
    Solution sol = Solution();
    for (auto &vertex : top_sort){
        for(Frequency freq : f) {
            if(sol.assings[vertex] == -1) { // Si no tiene ninguna frecuencia asignada.
                sol.apply_assign({vertex, freq.number_id});
            } else {
                if(sol.cost_of_assign({vertex, freq.number_id}) < 0) {
                    sol.apply_assign({vertex, freq.number_id});
                }
            }
        }
    }

    return sol;
}

Solution greedy_randomized_construction(double &a) {
    //TODO: Esquema de greedy_randomized_construction
    // Definir C conjunto total de posibles asignaciones  N * T
    // Definir RCL conjunto de mejores K posibles asignaciones, restringida por K y ordenada por calidad
    // La calidad se puede restringir de acuerdo a: C(e) inlcuido en {C^{min}, C^{min} + a(C^{max} - C^{min})}
    // Con a = 0 se vuelve un algoritmo completamente greedy, mientras que con a = 1 se vuelve una estrategia aleatoria.
    // Se selecciona aleatoriamente un elemento de RCL y se usa como solucion.
    // Se actualiza RCL recreandolas, donde se eliminan todas las posibles convinaciones con el vertice asignado

    //TODO: Solucion
    Solution sol = Solution();
    vpi possibles = C; // copia de C, todas las posibles asignaciones de la instancia
    while(!sol.is_complete()) {
        auto RCL = sol.get_RCL(possibles, a); // ordena possibles por costo incremental y devuelve los primeros K pares de asignaciones
        uniform_int_distribution<int> dist(0, RCL.size());
        int rand_pos = dist(mt);
        int count = 0;
        while(count != rand_pos) { RCL.pop(); count++; }
        pi current_assign = RCL.top().second; // Selecciona  un elemento aleatorio de RCL
        sol.apply_assign(current_assign);

        possibles.erase(remove_if(possibles.begin(), possibles.end(),  [&current_assign](pi &pair) { return pair.first == current_assign.first ;}), possibles.end()); // Elimina todas aquellas posibles asignaciones al vertice current_assign.first
    }

    return  sol;
}

Solution local_search(Solution &sol) {
    for(int i = 0; i < N; i ++) {
        if(!graph[i].empty()) { // Si el vertice tiene vecinos, sino no, no tiene sentido cambiarle el color
            for(int f = 0; f < T; f++) {
                if(sol.assings[i] != f) {
                    // Si es menor a 0 es porque esta asignacion reduce el costo total
                    if(sol.cost_of_assign({i, f}) < 0) {
                        sol.apply_assign({i, f});
                    }
                }
            }
        }
    }

    return sol;
}

bool stop_criteria() { return current_iteration >= maximum_iterations || time(nullptr) >= start_time + maximum_time; }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////// AUX PATH RELINKING /////////////////////////////////////////
int max_elites = 4; // Numero de maxima cantidad de elites
vector<Solution> elite_vector;

int get_position_of_worst_elite() {
    int pos = 0;
    int cost = inf;
    for(int i = 0; i < elite_vector.size(); i++) {
        if (elite_vector[i].cost > cost) {
            pos = i;
            cost = elite_vector[i].cost;
        }
    }
    return pos;
}

priority_queue<pair<int, pi>, vector<pair<int, pi>>, Less_compare_assign > best_differences_assign(Solution &current_sol, Solution &elite_sol) {
    priority_queue<pair<int, pi>, vector<pair<int, pi>>, Less_compare_assign> p_queue;
    for(int i = 0; i < N; i++) {
        if(current_sol.assings[i] != elite_sol.assings[i]) {
            int current_cost = current_sol.cost_of_assign({i, elite_sol.assings[i]});
            p_queue.push({current_cost, {i, elite_sol.assings[i]}});
        }
    }

    return p_queue;
}

void add_or_replace_elites(Solution &sol) {
    if(elite_vector.size() == max_elites) {
        int pos = get_position_of_worst_elite();
        if(sol.cost < elite_vector[pos].cost) { elite_vector[pos] = sol; }
    } else { elite_vector.push_back(sol); }
}

Solution path_relinking(Solution &current_sol, Solution &elite_sol) {
    auto queue = best_differences_assign(current_sol, elite_sol);
    auto best_solution = (current_sol.cost < elite_sol.cost)? current_sol  : elite_sol;
    auto new_current_solution = current_sol;

    while(!queue.empty()) {
        pi assign = queue.top().second;
        queue.pop();

        new_current_solution.apply_assign(assign);
        new_current_solution = local_search(new_current_solution);
        if(new_current_solution.cost < best_solution.cost) { best_solution = new_current_solution; }
    }
    return best_solution;
}
///////////////////////////////////////// FIN AUX PATH RELINKING /////////////////////////////

///////////////////////////////////////// AUX REACTIVE GRASP /////////////////////////////////
void re_calculate_probabilities(int &best_current_cost) {
    for (int i = 0; i < cost_averages.size(); i++) {
        if(!all_cost_for_each_a[i].empty()) {
            cost_averages[i] = accumulate(all_cost_for_each_a[i].begin(), all_cost_for_each_a[i].end(), 0) / all_cost_for_each_a[i].size(); // calcula el promedio de los costos con ese valor de a
        }
    }

    for(int q = 0; q < quality_of_average_costs.size(); q++) {
        if(cost_averages[q] != 0) {
            quality_of_average_costs[q] = static_cast<double>(best_current_cost) / cost_averages[q];
        }
    }

    for(int p = 0; p < probabilities.size(); p++) {
        auto summ_of_averages = accumulate(quality_of_average_costs.begin(), quality_of_average_costs.end(), 0.0);
        if(summ_of_averages != 0 && quality_of_average_costs[p] != 0) {
            probabilities[p] = quality_of_average_costs[p] / summ_of_averages;
        }
    }
}

int get_random_pos_of_a() {
    std::discrete_distribution<int> distribution (probabilities.begin(), probabilities.end());
    return distribution(mt);
}
///////////////////////////////////////////////// FIN AUX REACTIVE GRASP ////////////////////////////

///////////////////////////////////////////////// GRASP /////////////////////////////////////////////
Solution grasp() {
    initialize_all_possible_assignments();

    uniform_real_distribution<double> dist(0.0, 1.0);
    start_time = time(0);
    current_iteration = 0;

    Solution global_solution = greedy_solution();

    while(!stop_criteria()) {
        double a = dist(mt); // Con a = 0 se vuelve un algoritmo completamente greedy, mientras que con a = 1 se vuelve una estrategia aleatoria
        Solution current_solution = greedy_randomized_construction(a);
        current_solution = local_search(current_solution);
        if(current_solution.cost < global_solution.cost) {
            cout << "Mejore " << global_solution.cost << " por " << current_solution.cost << endl;
            global_solution = current_solution;
        }
        current_iteration++;
    }

    cout << "Cantidad de iteraciones: " << current_iteration << endl;
    return global_solution;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////// REACTIVE GRASP /////////////////////////////////////
Solution reactive_grasp() {
    initialize_all_possible_assignments();
    start_time = time(0);
    current_iteration = 0;

    Solution global_solution = greedy_solution();

    while(!stop_criteria()) {
        if(current_iteration % count_of_iteration_for_recalculate == 0) { re_calculate_probabilities(global_solution.cost); }
        int pos_a = get_random_pos_of_a();
        double a = possibles_a[pos_a]; // Con a = 0 se vuelve un algoritmo completamente greedy, mientras que con a = 1 se vuelve una estrategia aleatoria
        Solution current_solution = greedy_randomized_construction(a);
        current_solution = local_search(current_solution);

        all_cost_for_each_a[pos_a].push_back(current_solution.cost); // Agrega el costo de la solucion actual al vector de costos para esa posicion de a

        if(current_solution.cost < global_solution.cost) {
            cout << "Mejore " << global_solution.cost << " por " << current_solution.cost << endl;
            global_solution = current_solution;
        }
        current_iteration++;
    }

    cout << "Cantidad de iteraciones: " << current_iteration << endl;
    return global_solution;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////// PATH RELINKING ////////////////////////////////////////
Solution grasp_with_path_relinking() {
    initialize_all_possible_assignments();
    uniform_real_distribution<double> dist_a(0.0, 1.0);
    start_time = time(0);
    current_iteration = 0;

    Solution global_solution = greedy_solution();
    elite_vector.push_back(global_solution);

    while(!stop_criteria()) {
        double a = dist_a(mt); // Con a = 0 se vuelve un algoritmo completamente greedy, mientras que con a = 1 se vuelve una estrategia aleatoria
        Solution current_solution = greedy_randomized_construction(a);
        current_solution = local_search(current_solution);

        uniform_int_distribution<int> dist_elites(0, elite_vector.size()-1);
        int rand_pos_elite = dist_elites(mt);
        current_solution = path_relinking(current_solution, elite_vector[rand_pos_elite]);
        if(current_solution.cost < global_solution.cost) {
            add_or_replace_elites(current_solution);
            cout << "Mejore " << global_solution.cost << " por " << current_solution.cost << endl;
            global_solution = current_solution;
        }
        current_iteration++;
    }

    cout << "Cantidad de iteraciones: " << current_iteration << endl;
    return global_solution;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////// REACTIVE GRASP + PATH RELINKING /////////////////////////////////
Solution reactive_grasp_with_path_relinking() {
    initialize_all_possible_assignments();
    start_time = time(0);
    current_iteration = 0;

    Solution global_solution = greedy_solution();
    elite_vector.push_back(global_solution);

    while(!stop_criteria()) {
        if(current_iteration % count_of_iteration_for_recalculate == 0) { re_calculate_probabilities(global_solution.cost); }
        int pos_a = get_random_pos_of_a();
        double a = possibles_a[pos_a]; // Con a = 0 se vuelve un algoritmo completamente greedy, mientras que con a = 1 se vuelve una estrategia aleatoria
        Solution current_solution = greedy_randomized_construction(a);
        current_solution = local_search(current_solution);

        all_cost_for_each_a[pos_a].push_back(current_solution.cost); // Agrega el costo de la solucion actual al vector de costos para esa posicion de a

        uniform_int_distribution<int> dist_elites(0, elite_vector.size()-1);
        int rand_pos_elite = dist_elites(mt);
        current_solution = path_relinking(current_solution, elite_vector[rand_pos_elite]);

        if(current_solution.cost < global_solution.cost) {
            add_or_replace_elites(current_solution);
            cout << "Mejore " << global_solution.cost << " por " << current_solution.cost << endl;
            global_solution = current_solution;
        }
        current_iteration++;
    }

    cout << "Cantidad de iteraciones: " << current_iteration << endl;
    return global_solution;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    std::string filename = R"(..\instances\miles1500.colcep)";
    std::ifstream istrm(filename);

    if(istrm.is_open()) {
        istrm >> N;
        istrm >> T;
        istrm >> M;

        f = vector<Frequency>();
        cost_of_freqs = vi();
        cost_conflict_matrix = vector<vi>(N, vector<int>(N, 0));

        for (auto i = 0; i < T; i++) {
            istrm >> current;
            f.push_back({current, i}); // {current, i} :: Frequency
            cost_of_freqs.push_back(current);
        }

        // Se ordena f por costo de uso
        sort(f.begin(), f.end(), order_func);

        graph = vector<vi>(N, vi());

        for (auto e = 0; e < M; e++) {
            istrm >> i;
            istrm >> j;
            istrm >> cost;

            // Indices desde 0 hasta N-1
            i--;
            j--;

            cost_conflict_matrix[i][j] = cost;
            cost_conflict_matrix[j][i] = cost;

            graph[i].push_back(j);
            graph[j].push_back(i);
        }

        grasp_with_path_relinking().show_solution();
    }

    return 0;
}