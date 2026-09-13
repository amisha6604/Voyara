#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// A single directed edge: a flight leg.
// Kept deliberately close to the JSON schema in data/flights.json so loading is direct.
struct Edge {
    std::string flight_id;
    std::string destination;   // airport_id this edge leads to
    std::string airline;
    int price_inr;
    int duration_minutes;
    std::string departure;     // ISO datetime string, local to origin airport
    std::string arrival;       // ISO datetime string, local to destination airport
    // TODO (later, time-dependent routing step): parse these into a real time type
    // instead of comparing strings. Not needed for plain Dijkstra.
};

struct Airport {
    std::string airport_id;
    double latitude;
    double longitude;
    // add more fields (city, timezone) only if/when you actually need them in the engine
};

// Adjacency-list graph. Why adjacency list and not a matrix: see
// docs/graph_theory_and_dijkstra_notes.md section 2 before you implement this.
class Graph {
public:
    // TODO: load airports from a JSON file into `airports`
    void loadAirports(const std::string& filepath);

    // TODO: load flights from a JSON file, building the adjacency list.
    // Each flight becomes one directed edge: adjacency[origin].push_back(edge)
    void loadFlights(const std::string& filepath);

    // TODO: return the list of outgoing edges for a given airport_id.
    // Return an empty vector if the airport has no outgoing flights
    // (this is how the disconnected-node case, e.g. IXZ, should behave —
    // not an error, just zero edges).
    const std::vector<Edge>& getEdges(const std::string& airport_id) const;

    // TODO: return true if this airport_id exists in the graph at all.
    bool hasAirport(const std::string& airport_id) const;

    size_t numAirports() const { return airports.size(); }

private:
    std::unordered_map<std::string, Airport> airports;
    std::unordered_map<std::string, std::vector<Edge>> adjacency;
};
    