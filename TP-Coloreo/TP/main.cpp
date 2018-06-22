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
int T; // numero de frecuecncias
int N; // numero de antenas disponibles
int M; // numero de conflictos posibbles por asignar con la misma frecuencia
mi graph; // lista de adyacencias
vector<Frequency> f; // vector de Frequency ordenado por costo
vi cost_of_freqs; // indica el costo de usar la frecuencia i
mi cost_conflict_matrix; // costo de colisiones de frecuencias para el par de antenas i, k


///////////////////////////////////////////// Global variables for greedy randomized construction //////////
vpi C; // Vector de pares de todas las posibles asignacinoes
int K = 4; // Cantidad de elementos maximo de CRL
unsigned int seed = 23;
mt19937 mt(seed);
////////////////////////////////////////////// Stop Criteria ///////////////////////////////////////////////
int maximum_iterations = 10000;
int current_iteration;
int maximum_time = 300; // 5 minutos
time_t start_time;

///////////////////////////////////////////// Order function //////////////////////////////////////////////
bool order_func (Frequency i, Frequency j) { return (i.cost_of_use < j.cost_of_use); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////// Solution /////////////////////////////////////////////////
struct Greater_compare_assign{ bool operator()(const pair<int, pi> &t1, const pair<int, pi> &t2) const { return t1.first < t2.first; } };

struct Solution {
    vi used_frequencies; // used_frequencies[i] indica cuantas antenas usan la frecuencia i
    vi assings; // assings[i] indica la frecuencia asignada al vertice i.
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

    if (current_assign == -1) { return 0; } // si la anterior asignacion era -1, no generaba ningun costo tal asignacion
    if (used_frequencies[current_assign] == 1) current_cost += cost_of_freqs[ current_assign ]; // si la frecuencia solo estaba usada por este vertice
    else
        for(auto &u : graph[vertex]) { if (assings[u] == current_assign) { current_cost += cost_conflict_matrix[u][vertex]; } }

    return current_cost;
}

inline int Solution::cost_of_new_assign(int &vertex, int &new_assign) {
    int current_cost = 0;

    if (new_assign == -1) { return 0; }

    if(used_frequencies[new_assign] == 0) current_cost += cost_of_freqs[new_assign]; // si la frecuencia no esta usada
    else
        for(auto &u : graph[vertex]) { if(assings[u] == new_assign) { current_cost += cost_conflict_matrix[u][vertex]; } }

    return current_cost;
}

int Solution::cost_of_assign(pi i) {
    // al costo de la nueva asignacion le resto el costo de la vieja asignacion. Por lo tanto, si es negativo, el la solucion es mejor
    return cost_of_new_assign(i.first, i.second) - cost_of_current_assign(i.first, i.second);
}

void Solution::show_solution() { // por ahora no escribe en files
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
            if(sol.assings[vertex] == -1) { // si no tiene ninguna frecuencia asignada.
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

vector<Solution> elite_vector;

Solution path_relinking(Solution &current_sol, Solution &elite_sol) {
    auto queue = best_differences_assign(current_sol, elite_sol);

    int iterations = 0;
    while(iterations != N && queue.size() > 0) {
        Solution new_sol = current_sol;
        pi assign = queue.top();
        queue.pop();
        new_sol.apply_assign(assign);
        if(new_sol.cost < current_sol.cost) {
            current_sol = new_sol;
        }
        add_or_replace_elites(elite_vector, new_sol);
    }
    current_sol;
}

bool stop_criteria() { return current_iteration >= maximum_iterations || time(nullptr) >= start_time + maximum_time; }

Solution grasp() {
    initialize_all_possible_assignments();

    uniform_real_distribution<double> dist(0.0, 1.0);
    start_time = time(0);
    current_iteration = 0;

    Solution global_solution = greedy_solution();
    elite_vector.push_back(global_solution);

    while(!stop_criteria()) {
        double a = dist(mt); // Con a = 0 se vuelve un algoritmo completamente greedy, mientras que con a = 1 se vuelve una estrategia aleatoria
        Solution current_solution = greedy_randomized_construction(a);
        current_solution = local_search(current_solution);
        current_solution = path_relinking(current_solution, elite_vector[rand_pos]);
        if(current_solution.cost < global_solution.cost) {
            add_or_replace_elites(, current_solution);
            global_solution = current_solution;
        }
        current_iteration++;
    }

    cout << "Cantidad de iteraciones: " << current_iteration << endl;
    return global_solution;
}

int main() {
    std::string filename = R"(..\instances\miles500.colcep)";
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

        grasp().show_solution();
    }

    return 0;
}