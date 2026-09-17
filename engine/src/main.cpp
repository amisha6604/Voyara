#include <iostream>
#include <string>
#include "graph.h"
#include "dijkstra.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

int main() {
    Graph g;
    g.loadAirports("../data/airports.json");
    g.loadFlights("../data/flights.json");

    // Read JSON request from stdin
    string input;
    getline(cin, input);

    // Parse JSON
    json request = json::parse(input);

    // Extract values
    string origin = request["origin"];
    string destination = request["destination"];
    string weightType = request["weightType"];

    // Convert string to WeightType enum
    WeightType weight;

    if (weightType == "PRICE") {
        weight = WeightType::PRICE;
    } else {
        weight = WeightType::DURATION;
    }

    

    

    auto result = dijkstra(g, origin, destination, weight);
   
    json response;

response["found"] = result.found;

if (result.found) {
    response["totalCost"] = result.totalCost;
    response["path"] = result.path;
    response["flightIds"] = result.flightIds;
}

response["nodesExplored"] = result.nodesExplored;
response["runtimeMs"] = result.runtimeMs;

cout << response.dump() << endl;    
    return 0;
}
