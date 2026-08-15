#pragma once
#include <string>
#include <vector>
#include "graph.h"

// What you're optimizing "weight" by. Plain Dijkstra needs ONE weight per edge —
// price OR duration, not both at once. (Combining both into a single weighted
// score comes later, in the scoring layer — see spec Section 9.3 on why raw
// attributes are kept separate from the objective.)
enum class WeightType {
    PRICE,
    DURATION
};

struct PathResult {
    bool found = false;
    double totalCost = 0.0;                    // in whatever unit WeightType implies
    std::vector<std::string> path;              // airport_ids, origin -> destination in order
    std::vector<std::string> flightIds;         // flight_ids taken, same order

    // benchmarking fields (spec Section 25) — fill these in as you implement,
    // you'll need them for the Dijkstra vs A* comparison later
    long long nodesExplored = 0;
    double runtimeMs = 0.0;
};

// TODO: implement in dijkstra.cpp
//
// Before you write a single line: re-read docs/graph_theory_and_dijkstra_notes.md
// sections 3 and 4, then answer out loud:
//   - what goes IN the priority queue? (hint: a pair of (cost, airport_id))
//   - how do you avoid reprocessing an already-finalized node?
//   - where does `previous[]` get updated, and what does it need to store to let
//     you reconstruct BOTH the airport path and the flight_id path?
//
// Test this against the known edge cases in your seed data BEFORE trusting it:
//   1. VNS -> DEL  (should find a path, multiple options exist)
//   2. VNS -> IXZ  (should return found=false, IXZ is disconnected)
//   3. VNS -> VNS  (should return totalCost = 0, trivial path)
//   4. DEL -> BOM  (two flights exist at similar times/prices — verify you get
//      a valid one, doesn't have to be a specific one, but cost should match
//      the cheaper of the two if optimizing PRICE)
PathResult dijkstra(const Graph& graph,
                     const std::string& originId,
                     const std::string& destinationId,
                     WeightType weightType);
