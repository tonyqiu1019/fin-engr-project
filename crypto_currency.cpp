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
    double *dist = new double[n*n];
    int *nexti = new int[n*n];

    for (int i=0; i<n; i++) for (int j=0; j<n; j++) {
        dist[i*n+j] = i == j ? 0 : DBL_MAX;
        nexti[i*n+j] = -1;

        if (edges.find(i * MAXN + j) != edges.end()) {
            dist[i*n+j] = edges[i * MAXN + j];
            nexti[i*n+j] = j;
        } else if (inter.find(i * MAXN + j) != inter.end()) {
            dist[i*n+j] = log(1.0/exrate);
            nexti[i*n+j] = j;
        }
    }

    for (int k=0; k<n; k++) {
        for (int i=0; i<n; i++) for (int j=0; j<n; j++) {
            if (dist[i*n+k] + dist[k*n+j] + PRECISION < dist[i*n+j]) {
                dist[i*n+j] = dist[i*n+k] + dist[k*n+j];
                nexti[i*n+j] = nexti[i*n+k];
            }
        }
        for (int i=0; i<n; i++) {
            if (dist[i*n+i] < -PRECISION) {
                vector<int> ret;
                if (nexti[i*n+i] == -1) {
                    cerr << "something went wrong" << endl; exit(2);
                }
                ret.push_back(i);
                int cur = nexti[i*n+i], target = i;
                while (cur != target) {
                    ret.push_back(cur);
                    cur = nexti[cur*n+target];
                }
                ret.push_back(i);
                delete[] dist; delete[] nexti;
                return ret;
            }
        }
    }

    delete[] dist; delete[] nexti;
    return vector<int>(0);
}


void printCycle(vector<int> &cycle, vector<string> &idx_name,
    unordered_map<int, double> &rates)
{
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


void rmArbitrage(vector<string> &idx_name,
    unordered_map<int, double> &edges, unordered_set<int> &inter, int n,
    unordered_map<int, double> &rates)
{
    while (true) {
        vector<int> cycle = floydWarshall(edges, inter, n, 0.0);
        if (cycle.empty()) break;

        string curr = idx_name[cycle[0]];
        string ex = curr.substr(curr.find("-") + 1);
        cout << ex << " contains internal arbitrage, ignoring:" << endl;
        printCycle(cycle, idx_name, rates); cout << endl;

        unordered_map<int, double> tmp1;
        for (auto p: edges) {
            string src = idx_name[p.first / MAXN];
            string dst = idx_name[p.first % MAXN];
            if (src.find(ex) != -1 || dst.find(ex) != -1) continue;
            tmp1[p.first] = p.second;
        }
        edges = tmp1;

        unordered_set<int> tmp2;
        for (int i: inter) {
            string src = idx_name[i / MAXN];
            string dst = idx_name[i % MAXN];
            if (src.find(ex) != -1 || dst.find(ex) != -1) continue;
            tmp2.insert(i);
        }
        inter = tmp2;
    }
}


void detectCycle(vector<string> &idx_name,
    unordered_map<int, double> &edges, unordered_set<int> &inter,
    unordered_map<int, double> &rates, int n, double exrate)
{
    cout << "Detecting arbitrage with rate: " << exrate << endl;
    vector<int> cycle = floydWarshall(edges, inter, n, exrate);
    if (cycle.empty()) cout << "No arbitrage exists" << endl;
    else printCycle(cycle, idx_name, rates);
}


void findNoArbitrageRate(unordered_map<int, double> &edges,
    unordered_set<int> &inter, int n)
{
    cout << "Finding no-arbitrage rate" << endl;
    double high = 1.0, low = 0.0, mid;
    while (high - low > PRECISION) {
        mid = high / 2 + low / 2;
        cout << "Current searching for rate: " << mid << endl;
        vector<int> cycle = floydWarshall(edges, inter, n, mid);
        if (cycle.empty()) low = mid;
        else high = mid;
    }
    cout << "Final rate should be: " << (high+low) / 2 << endl;
}


int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        cerr << "incorrect format of arguments" << endl; exit(1);
    }
    string arg(argv[1]);
    bool detect = false; double exrate = 1.0;
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
            rates[name_idx[src] * MAXN + name_idx[dst]] = exrate;
        } else {
            edges[name_idx[src] * MAXN + name_idx[dst]] = log(1.0/rate);
            rates[name_idx[src] * MAXN + name_idx[dst]] = rate;
        }
    }

    int n = idx_name.size();
    rmArbitrage(idx_name, edges, inter, n, rates);
    cout << "Number of currencies in exchanges: " << n << endl;

    if (detect) detectCycle(idx_name, edges, inter, rates, n, exrate);
    else findNoArbitrageRate(edges, inter, n);
}
