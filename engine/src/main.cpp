#include <iostream>
#include "graph.h"
#include "dijkstra.h"
using namespace std;

int main() {
    Graph g;
    g.loadAirports("../data/airports.json");
    g.loadFlights("../data/flights.json");

    cout << "Loaded " << g.numAirports() << " airports" << endl;

    cout << "Has VNS? " << g.hasAirport("VNS") << endl;
    cout << "Has IXZ (disconnected)? " << g.hasAirport("IXZ") << endl;
    cout << "Has FAKE? " << g.hasAirport("FAKE") << endl;

    auto edges = g.getEdges("VNS");
    cout << "VNS has " << edges.size() << " outgoing flights:" << endl;
    for (auto& e : edges) {
        cout << "  " << e.flight_id << " -> " << e.destination
             << " (Rs " << e.price_inr << ", " << e.duration_minutes << " min)" << endl;
    }

    auto result = dijkstra(g, "VNS", "BLR", WeightType::PRICE);
   
    if (result.found) {
        cout << "Found path from VNS to BLR with total cost Rs " << result.totalCost << endl;
        cout << "Path: ";
        for (const auto& airport : result.path) {
            cout << airport << " ";
        }
        cout << endl;
        cout << "Flights: ";
        for (auto& flight : result.flightIds) {
            cout << flight << " ";
        }
        cout << endl;
    } else {
        cout << "No path found from VNS to BLR" << endl;
    }       
    return 0;
}
