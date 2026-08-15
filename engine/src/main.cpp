#include <iostream>
#include "graph.h"
#include "dijkstra.h"

int main() {
    std::cout << "Travel Optimization Engine - C++ core\n";
    std::cout << "This compiles, which means your toolchain (g++, Makefile) works.\n";
    std::cout << "Next: implement Graph::loadAirports/loadFlights in src/graph.cpp,\n";
    std::cout << "then implement dijkstra() in src/dijkstra.cpp.\n";

    // Once graph.cpp is implemented, this becomes your first real test:
    //
    // Graph g;
    // g.loadAirports("../data/airports.json");
    // g.loadFlights("../data/flights.json");
    // std::cout << "Loaded " << g.numAirports() << " airports\n";
    //
    // PathResult result = dijkstra(g, "VNS", "DEL", WeightType::PRICE);
    // if (result.found) {
    //     std::cout << "Cheapest VNS->DEL: " << result.totalCost << " INR\n";
    // } else {
    //     std::cout << "No path found\n";
    // }

    return 0;
}
