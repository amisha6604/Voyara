#include "graph.h"
#include <fstream>
#include <iostream>

// Implement Graph::loadAirports, loadFlights, getEdges, hasAirport here.
//
// You'll need a JSON library to parse data/airports.json and data/flights.json.
// Recommended: nlohmann/json (single header file, very beginner-friendly).
// Install (inside WSL/Ubuntu):
//   cd engine/include
//   wget https://github.com/nlohmann/json/releases/latest/download/json.hpp
// Then: #include "json.hpp"  and  using json = nlohmann::json;
//
// Read a file into a json object like this:
//   std::ifstream f(filepath);
//   json data = json::parse(f);
//   for (auto& item : data) { ... item["airport_id"] ... }
