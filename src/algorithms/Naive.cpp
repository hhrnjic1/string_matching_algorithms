#include "Algorithms.h"

using namespace std;

// Naivni algoritam za podudaranje stringova
vector<int> naiveSearch(const string& text, const string& pattern) {
    vector<int> positions;
    int n = text.length();
    int m = pattern.length();
    
    // Provjera svakog mogućeg podudaranja
    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            if (text[i + j] != pattern[j])
                break;
        }
        
        if (j == m) // Nađeno podudaranje
            positions.push_back(i);
    }
    
    return positions;
}