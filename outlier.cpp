#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    string line; bool between = false;
    vector<string> buf;

    while (getline(cin, line)) {
        if (line.find("*CASH*") != -1 &&
            line.substr(line.find("*CASH*")+6).find("*CASH*") == -1 &&
            line.find("0.9999") == -1)
        {
            buf.push_back(line);
        }
        else if (line.find("VWOX") == -1)
        {
            if (!between) cout << line << endl;
            if (line == "- - 0") between = true;
            else {
                if (line.find("XBT") != -1 || line.find("XRP") != -1 ||
                    line.find("LTC") != -1 || line.find("ETH") != -1)
                    cout << line << endl;
            }
        }
    }

    for (string line2 : buf) cout << line2 << endl;
}
