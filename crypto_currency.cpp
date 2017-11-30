#include <cfloat>
#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define MAXN 10000
#define PRECISION 1e-4

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
                if (nexti[i][i] == -1) {
                    cerr << "something went wrong" << endl; exit(2);
                }
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

    return vector<int>(0);
}

void detectCycle(vector<string> &idx_name,
    unordered_map<int, double> &edges, unordered_set<int> &inter,
    unordered_map<int, double> &rates, int n, double exrate)
{
    vector<int> cycle = floydWarshall(edges, inter, n, exrate);
    if (cycle.empty()) {
        cout << "No arbitrage exists" << endl;
    } else {
        cout << "Number of transactions: " << cycle.size()-1 << endl;
        double prod = 1.0;
        for (int i=0; i<cycle.size()-1; i++) {
            // cout << rates[cycle[i] * MAXN + cycle[i+1]] << endl;
            prod *= rates[cycle[i] * MAXN + cycle[i+1]];
        }
        cout << "Profit: " << prod << " per 1 unit currency" << endl;
        cout << "The transactions are: ";
        for (int i=0; i<cycle.size(); i++) {
            cout << idx_name[cycle[i]];
            if (i != cycle.size()-1) cout << " -> ";
        }
        cout << endl;
    }
}

void findNoArbitrageRate(unordered_map<int, double> &edges,
    unordered_set<int> &inter, int n)
{
    double high = 1.0, low = 0.0, mid;
    while (high - low > PRECISION) {
        mid = high / 2 + low / 2;
        cout << "current rate: " << mid << endl;
        vector<int> cycle = floydWarshall(edges, inter, n, mid);
        if (cycle.empty()) low = mid;
        else high = mid;
    }
    cout << "Final rate should be: " << low << endl;
}

int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        cerr << "incorrect format of arguments" << endl; exit(1);
    }
    string arg(argv[1]);
    bool detect = false; double exrate = 1;
    if (arg == "detect") {
        detect = true;
        if (argc == 3) exrate = atof(argv[2]);
    } else if (argc == 3 || arg != "find") {
        cerr << "incorrect format of arguments" << endl; exit(1);
    }

    string src, dst; double rate;
    bool between = false;
    vector<string> idx_name; unordered_map<string, int> name_idx;
    unordered_map<int, double> edges;
    unordered_set<int> inter;
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
    cout << "Number of currencies in exchanges: " << n << endl;

    if (detect) detectCycle(idx_name, edges, inter, rates, n, exrate);
    else findNoArbitrageRate(edges, inter, n);
}
