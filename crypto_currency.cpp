#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <cfloat>

#define MAXN 10000

using namespace std;

vector<int> floydWarshall(unordered_map<int, double> &edges,
    unordered_set<int> &inter, int n, double exrate)
{
    double dist[n][n]; int nexti[n][n];
    for (int i=0; i<n; i++) for (int j=0; j<n; j++) {
        dist[i][j] = i == j ? 0 : DBL_MAX;
        nexti[i][j] = -1;

        if (edges.find(i * MAXN + j) != edges.end()) {
            dist[i][j] = edges[i * MAXN + j];
            nexti[i][j] = j;
        } else if (inter.find(i * MAXN + j) != inter.end()) {
            dist[i][j] = log(1.0/exrate);
            nexti[i][j] = j;
        }
    }

    for (int k=0; k<n; k++) {
        for (int i=0; i<n; i++) for (int j=0; j<n; j++) {
            if (dist[i][k] + dist[k][j] < dist[i][j]) {
                dist[i][j] = dist[i][k] + dist[k][j];
                nexti[i][j] = nexti[i][k];
            }
        }
        for (int i=0; i<n; i++) {
            if (dist[i][i] < 0) {
                // cout << dist[i][i] << " here" << endl;
                vector<int> ret;
                if (nexti[i][i] == -1) throw "something is wrong";
                ret.push_back(i);
                int cur = nexti[i][i], target = i;
                while (cur != target) {
                    ret.push_back(cur);
                    cur = nexti[cur][target];
                }
                ret.push_back(i);
                return ret;
            }
        }
    }

    // for (int i=0; i<n; i++) {
    //     for (int j=0; j<n; j++) cout << dist[i][j] << "\t";
    //     cout << endl;
    // }

    return vector<int>(0);
}

int main() {
    string src, dst; double rate;
    bool between = false;

    vector<string> idx_name; unordered_map<string, int> name_idx;
    unordered_map<int, double> edges; unordered_set<int> inter;
    unordered_map<int, double> rates;

    while (cin >> src >> dst >> rate) {
        if (src == "-" && dst == "-") {
            between = true;
            continue;
        }

        if (name_idx.find(src) == name_idx.end()) {
            name_idx[src] = idx_name.size();
            idx_name.push_back(src);
        }
        if (name_idx.find(dst) == name_idx.end()) {
            name_idx[dst] = idx_name.size();
            idx_name.push_back(dst);
        }

        if (between) {
            inter.insert(name_idx[src] * MAXN + name_idx[dst]);
            rates[name_idx[src] * MAXN + name_idx[dst]] = 1.0;
        } else {
            edges[name_idx[src] * MAXN + name_idx[dst]] = log(1.0/rate);
            rates[name_idx[src] * MAXN + name_idx[dst]] = rate;
        }
    }

    int n = idx_name.size();
    vector<int> cycle = floydWarshall(edges, inter, n, 1);

    if (cycle.empty()) {
        cout << "No arbitrage exists" << endl;
    } else {
        cout << "Number of transactions: " << cycle.size()-1 << endl;
        double prod = 1.0;
        for (int i=0; i<cycle.size()-1; i++) {
            prod *= rates[cycle[i] * MAXN + cycle[i+1]];
        }
        cout << "Profit is: " << prod << " per 1 unit currency" << endl;
        cout << "The transactions are: ";
        for (int i=0; i<cycle.size(); i++) {
            cout << idx_name[cycle[i]];
            if (i != cycle.size()-1) cout << " -> ";
        }
        cout << endl;
    }
}
