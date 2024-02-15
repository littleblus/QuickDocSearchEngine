#include "index/search.hpp"

const std::string raw_path = "../raw_html/raw_html.txt";

int main() {
    search::Search search;
    search.init(raw_path);

    std::string query;
    for (;;) {
        std::cout << "search> ";
        std::getline(std::cin, query);
        if (query == "quit") break;
        std::cout << search.search(query) << std::endl;
    }
    return 0;
}