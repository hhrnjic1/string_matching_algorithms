#pragma once
#include <vector>
#include <string>

std::vector<int> naiveSearch(const std::string& text, const std::string& pattern);
std::vector<int> kmpSearch(const std::string& text, const std::string& pattern);
std::vector<int> rabinKarpSearch(const std::string& text, const std::string& pattern);
std::vector<int> boyerMooreSearch(const std::string& text, const std::string& pattern);