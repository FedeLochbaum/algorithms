#include <vector>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <fstream>

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
mi complete_graph; // grafo completo
vector<Frequency> f;  // indica el costo de usar la frecuencia i
mi cost_conflict_matrix; // costo de colisiones de frecuencias para el par de antenas i, k


///////////////////////////////////////////// Global variables for greedy randomized construction //////////
vpi C; // Vector de pares de todas las posibles asignacinoes
double a = 0.5; // Con a = 0 se vuelve un algoritmo completamente greedy, mientras que con a = 1 se vuelve una estrategia aleatoria
int K = 4; // Cantidad de elementos maximo de CRL
unsigned int seed = 23;

////////////////////////////////////////////// Stop Criteria ///////////////////////////////////////////////
int maximum_iterations = 1000;
int current_iteration;
int maximum_time = 900; // 15 minutos
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
    void apply_assign(pi pair);
    int loss_for_collision();
    int cost_of_frequencies();

    // AUX
    void show_solution();
    void update_frequencies();
    bool is_complete();
    vpi remove_assigns_of(vpi &possibles, int vertex);
    void order_possibles(vpi &possibles);
    int cost_of_assign(pi i);
    bool order_assigns(pi &a, pi &b);
    vpi get_RCL(vpi &possibles);
};

Solution::Solution() {
    y = vi(T,0);
    assings = vi(N, -1);
}

int Solution::loss_for_collision(){
    auto copy_matrix = cost_conflict_matrix;
    int res = 0;

    for(int i = 0; i < copy_matrix.size(); i ++) {
        for(int k = 0; k < copy_matrix[i].size(); k++) {
            if(assings[i] == assings[k] && assings[i] != -1) {
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
    for(Frequency freq : f) { if(y[freq.number_id]) res += freq.cost_of_use; }

    return res;
}

void Solution::apply_assign(pi pair) {
    assings[pair.first] = pair.second;
    update_frequencies();
}

int Solution::functional() {
    return cost_of_frequencies() + loss_for_collision();
}

void Solution::update_frequencies() {
    y = vi(T,0);
    for (int assing : assings) { y[assing] = 1; }
}

bool Solution::is_complete(){
    for(auto assing : assings) {
        if(assing == -1) { return false; }
    }
    return true;
}

vpi Solution::remove_assigns_of(vpi &possibles, int vertex) {
    vpi res = vpi();

    std::copy_if (possibles.begin(), possibles.end(), std::back_inserter(res), [vertex](pi pair){
        return pair.first != vertex;
    } );

    return res;
}

vpi Solution::get_RCL(vpi &possibles) {
    order_possibles(possibles); // Ordeno de los pares por orden incremental de asignacion

    int c_min = cost_of_assign(possibles[0]); // minimo costo de usar las soluciones parciales
    int c_max = cost_of_assign(possibles[possibles.size()-1]); // maximo costo de usar las soluciones parciales
    pi range = {c_min, c_min + a*(c_max - c_min)};

    vpi res = vpi();

    for(auto pair : possibles) {
        if (res.size() == K) { break; }
        if((cost_of_assign(pair) - range.first) <= (range.second - range.first)) {
            res.push_back(pair);
        }
    }

    return res;
}

int Solution::cost_of_assign(pi i) { //TODO: hace demasiado lento el alogirtmo
    int res = functional();

    int current_assing = assings[i.first];
    apply_assign(i);

    res = functional() - res;

    assings[i.first] = current_assing;
    apply_assign({i.first, current_assing});

    return res;
}

bool Solution::order_assigns(pi &a, pi &b) { return cost_of_assign(a) < cost_of_assign(b); }

void Solution::order_possibles(vpi &possibles) {
    auto sol = *this;
    sort(possibles.begin(), possibles.end(), [&sol](pi &a, pi &b) { return sol.order_assigns(a, b); });
}

void Solution::show_solution() { // por ahora no escribe en files
    for(auto frec_id : assings) { cout << frec_id << endl; }
    cout << "Cost: " << functional() << endl;
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
    for (auto vertex : top_sort){
        Frequency last_freq_used{};
        for(Frequency freq : f) {
            if(sol.assings[vertex] == -1) { // si no tiene ninguna frecuencia asignada.
                sol.apply_assign({vertex, freq.number_id});
                last_freq_used = freq;
            } else {
                int current_cost = sol.functional();
                sol.apply_assign({vertex, freq.number_id});

                if(current_cost < sol.functional()) { // si no mejoro la anterior
                    sol.apply_assign({vertex, last_freq_used.number_id});
                } else {// si si mejoro el costo
                    last_freq_used = freq;
                }
            }
        }
    }

    return sol;
}

Solution greedy_randomized_construction() {
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
        auto RCL = sol.get_RCL(possibles); // ordena possibles por costo incremental y devuelve los primeros K pares de asignaciones
        int rand_pos = rand() % RCL.size();
        pi current_assign = RCL[rand_pos]; // Selecciona  un elemento aleatorio de RCL
        sol.apply_assign(current_assign);

        possibles = sol.remove_assigns_of(possibles, current_assign.first); // Elimina todas aquellas posibles asignaciones al vertice current_assign.first
    }

    cout << "Finalizo un randomized con costo: " << sol.functional() << endl;
    return  sol;
}

Solution local_search(Solution &sol) {
    for(int i = 0; i < N; i ++) {
        if(!graph[i].empty()) { // Si el vertice tiene vecinos, sino no, no tiene sentido cambiarle el color
            for(int f = 0; f < T; f++) {
                int old_freq = sol.assings[i];
                if(old_freq != f) {
                    int old_cost = sol.functional();
                    sol.apply_assign({i, f});

                    if(old_cost < sol.functional()) {
                        sol.apply_assign({i, old_freq});
                    }
                }
            }
        }
    }

    return sol;
}

bool stop_criteria() { return current_iteration >= maximum_iterations || time(nullptr) >= start_time + maximum_time; }

Solution grasp() {
    initialize_all_possible_assignments();
    start_time = time(0);
    current_iteration = 0;

    Solution global_solution = greedy_solution();
    int global_cost = global_solution.functional();
    cout << "Global cost: " << global_cost;

    while(!stop_criteria()) {
        Solution current_solution = greedy_randomized_construction();
        current_solution = local_search(current_solution);
        int current_cost = current_solution.functional();
        if(current_cost < global_cost) {
            cout << "Nueva solucion con costo: " << current_cost << endl;
            global_solution = current_solution;
            global_cost = current_cost;
        }
        current_iteration++;
    }

    return global_solution;
}

int main() {
    std::string filename = R"(..\instances\input_example.colcep)";
    std::ifstream istrm(filename);
    srand (seed);

    if(istrm.is_open()) {
        istrm >> N;
        istrm >> T;
        istrm >> M;

        f = vector<Frequency>();
        cost_conflict_matrix = vector<vi>(N, vector<int>(N, 0));

        for (auto i = 0; i < T; i++) {
            istrm >> current;
            f.push_back({current, i}); // {current, i} :: Frequency
        }

        // Se ordena f por costo de uso
        sort(f.begin(), f.end(), order_func);

        graph = vector<vi>(N, vi());
        complete_graph = vector<vi>(N, vi(N, 0));

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

            complete_graph[i][j] = 1;
            complete_graph[j][i] = 1;
        }

        grasp().show_solution();
    }

    return 0;
}