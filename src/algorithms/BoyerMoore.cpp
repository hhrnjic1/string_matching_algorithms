#include "Algorithms.h"
#include <algorithm>

using namespace std;

// Boyer-Moore algoritam - popravljen za rad sa binarnim podacima
vector<int> boyerMooreSearch(const string& text, const string& pattern) {
    vector<int> positions;
    int n = text.length();
    int m = pattern.length();
    
    if (m == 0 || m > n) return positions;
    
    // Priprema bad character heuristike sa zaštićenim pristupom
    vector<int> badChar(256, -1);
    
    // Koristimo unsigned char za binarne podatke
    for (int i = 0; i < m; i++) {
        unsigned char ch = static_cast<unsigned char>(pattern[i]);
        badChar[ch] = i;
    }
    
    // Pretraga sa dodatnim provjeram
    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;
        
        // Usporedba unazad
        while (j >= 0 && pattern[j] == text[s + j])
            j--;
        
        if (j < 0) {
            // Pronađeno podudaranje
            positions.push_back(s);
            
            // Pomjeri za sljedeći pokušaj
            if (s + m < n) {
                unsigned char nextChar = static_cast<unsigned char>(text[s + m]);
                s += m - badChar[nextChar];
            } else {
                s += 1;
            }
        } else {
            // Pomaći s koristeći bad character heuristiku
            unsigned char mismatchChar = static_cast<unsigned char>(text[s + j]);
            int shift = j - badChar[mismatchChar];
            s += max(1, shift);
        }
        
        // Dodatna provjera da ne idemo izvan granica
        if (s < 0) s = 1;
    }
    
    return positions;
}