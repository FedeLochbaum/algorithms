#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef vector<int> vi;
typedef vector<vi> mi;

int N, T, M, current, i, j, cost;
vi frequency;
mi cost_matrix;


int main() {

    cin >> N;
    cin >> T;
    cin >> M;

    frequency = vi(T);
    cost_matrix = vector<vi>(N, vector<int>(N));


    for(auto i = 0; i < T; i++) {
        cin >> current;
        frequency.push_back(current);
    }

    for(auto e = 0; e < M; e++) {
        cin >> i;
        cin >> j;
        cin >> cost;

        i--; // Uso indices desde 0 hasta N-1
        j--;

        cost_matrix[i][j] = cost;
        cost_matrix[j][i] = cost;
    }

    return 0;
}