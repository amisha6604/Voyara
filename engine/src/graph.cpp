#include "graph.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
using namespace std;

using json = nlohmann::json;

void Graph::loadAirports(const std::string& filepath) {
    ifstream f(filepath);
    json data = json::parse(f);

    for (auto& item : data) {
        Airport a;
        a.airport_id= item["airport_id"];
        a.latitude =item["latitude"];
        a.longitude = item["longitude"];

        airports[a.airport_id] = a;
    }
        
}

void Graph ::loadFlights(const std::string& filepath) {
    ifstream f(filepath);
    json data = json::parse(f);

    for(auto& item :  data) {
        Edge e;
        e.flight_id = item["flight_id"];
        e.destination = item["destination"];
        e.airline = item["airline"];
        e.price_inr = item["price_inr"];
        e.duration_minutes= item["duration_minutes"];
        e.departure= item["departure"];
        e.arrival= item["arrival"];

        string origin= item["origin"];
        adjacency[origin].push_back(e);
    }
}

const std:: vector<Edge> &Graph :: getEdges(const std::string& airport_id) const {
    static const vector<Edge> empty;
    auto it = adjacency.find(airport_id);
    if(it == adjacency.end()){
        return empty;
    }
    else return it->second;
}

bool Graph ::hasAirport(const std::string& airport_id) const{
    return airports.find(airport_id)==airports.end()?false:true;
}

void Graph::addAirport(const Airport& airport) {
    airports[airport.airport_id] = airport;
}

void Graph::addFlight(const std::string& origin, const Edge& edge) {
    adjacency[origin].push_back(edge);
}