#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;


vector<vector<int>> solutions;
int m;

int bc(int n, int k) {
    if(k > n) { return 0; }

    if(k ==0 && n ==0) { return 1; }

    if(solutions[n][k] == -2) {
        int res = 0;

        for(int i = 1; i <= m && n-i >= 0; i++) {
            res += bc(n-i, k-1);
        }

        solutions[n][k] = res;
    }

    return solutions[n][k];
}



int main() {
    int n, k;

    while(cin >> n) {
        cin >> k;
        cin >> m;

        solutions = vector<vector<int>>(n+1, vector<int>(k+1, -2));
        cout << std::to_string(bc(n, k)) << endl;
    }


    return 0;
}
