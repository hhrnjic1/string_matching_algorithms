#include "Algorithms.h"

using namespace std;

// Rabin-Karp algoritam - popravljeni za rad s binarnim podacima
vector<int> rabinKarpSearch(const string& text, const string& pattern) {
    vector<int> positions;
    int n = text.length();
    int m = pattern.length();
    
    const int prime = 101;  // Prost broj za hash funkciju
    const int d = 256;      // Broj mogućih karaktera
    
    // Računanje d^(m-1) % prime
    int h = 1;
    for (int i = 0; i < m - 1; i++)
        h = (h * d) % prime;
    
    int p = 0;  // Hash vrijednost za pattern
    int t = 0;  // Hash vrijednost za tekst
    
    // Izračunavanje početnih hash vrijednosti
    // POPRAVKA: Koristimo unsigned char za binarne podatke
    for (int i = 0; i < m; i++) {
        p = (d * p + (unsigned char)pattern[i]) % prime;
        t = (d * t + (unsigned char)text[i]) % prime;
    }
    
    // Pretraga
    for (int i = 0; i <= n - m; i++) {
        if (p == t) {
            // Provjera poklapanja karaktera
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            
            if (match)
                positions.push_back(i);
        }
        
        // Izračunavanje hash za sljedeći prozor
        // POPRAVKA: Koristimo unsigned char za binarne podatke
        if (i < n - m) {
            t = (d * (t - (unsigned char)text[i] * h) + (unsigned char)text[i + m]) % prime;
            if (t < 0)
                t += prime;
        }
    }
    
    return positions;
}