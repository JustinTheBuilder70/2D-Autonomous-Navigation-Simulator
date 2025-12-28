#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <type_traits>

// --- Universal Print Base Cases ---

// Simple types
template <typename T>
typename std::enable_if<!std::is_class<T>::value || std::is_same<T,std::string>::value>::type
universalPrint(const T& value) {
    std::cout << value;
}

// Pairs
template <typename T1, typename T2>
void universalPrint(const std::pair<T1,T2>& p) {
    std::cout << "(";
    universalPrint(p.first);
    std::cout << ", ";
    universalPrint(p.second);
    std::cout << ")";
}

// Containers (vector, set, map, etc.)
template <typename Container>
typename std::enable_if<!std::is_same<Container,std::string>::value &&
                        std::is_class<Container>::value>::type
universalPrint(const Container& c) {
    std::cout << "{ ";
    bool first = true;
    for (const auto& elem : c) {
        if (!first) std::cout << ", ";
        universalPrint(elem);
        first = false;
    }
    std::cout << " }";
}

// --- Variadic Multi-Arg Print ---


template <typename First, typename... Rest>
void print(const First& first, const Rest&... rest) {
    universalPrint(first);
    if constexpr(sizeof...(rest) > 0) {
        std::cout << " "; // space between args
        print(rest...);   // recursive call
    } else {
        std::cout << "\n"; // newline after last arg
    }
}