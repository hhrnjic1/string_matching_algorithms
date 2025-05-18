#include "Algorithms.h"
#include <algorithm>

using namespace std;

// Boyer-Moore algoritam
vector<int> boyerMooreSearch(const string& text, const string& pattern) {
    vector<int> positions;
    int n = text.length();
    int m = pattern.length();
    
    // Priprema bad character heuristike
    vector<int> badChar(256, -1);
    for (int i = 0; i < m; i++)
        badChar[pattern[i]] = i;
    
    // Pretraga
    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;
        
        while (j >= 0 && pattern[j] == text[s + j])
            j--;
        
        if (j < 0) {
            positions.push_back(s);
            s += (s + m < n) ? m - badChar[text[s + m]] : 1;
        } else {
            s += max(1, j - badChar[text[s + j]]);
        }
    }
    
    return positions;
}