#include "dijkstra.h"
#include <queue>
#include <unordered_map>
#include <limits>
#include <chrono>
#include <algorithm>
using namespace std;

PathResult dijkstra(const Graph& graph, const std::string& originId,
                     const std::string& destinationId, WeightType weightType) {
    PathResult result;
    auto startTime = chrono::high_resolution_clock::now();

    unordered_map<string, double> dist;
    unordered_map<string, pair<string, string>> previous; // airport -> (prev airport, flight_id used)
    unordered_map<string, bool> visited;

    // min-heap of (cost, airport_id). C++'s priority_queue is a MAX-heap by
    // default, so we store negative costs, OR use greater<> comparator.
    // Simplest for beginners: use pair<double,string> with greater<> comparator:
    priority_queue<pair<double,string>, vector<pair<double,string>>, greater<>> pq;

    dist[originId] = 0.0;
    pq.push({0.0, originId});

    long long nodesExplored = 0;

    while (!pq.empty()) {

        auto [currentCost, currentAirport] = pq.top();
        pq.pop();

        if(visited[currentAirport]) {
            continue;
        }

        visited[currentAirport] = true;
        nodesExplored++;

        if(currentAirport ==destinationId) break;

        for(const auto& edge : graph.getEdges(currentAirport)) {
            double edgeWeight = (weightType == WeightType::PRICE) ? edge.price_inr : edge.duration_minutes;
            double newCost = currentCost + edgeWeight;

            if(dist.find(edge.destination) == dist.end() || newCost < dist[edge.destination]) {
                dist[edge.destination] = newCost;
                previous[edge.destination] = {currentAirport, edge.flight_id};
                pq.push({newCost, edge.destination});
            }
        }
    }

    if(dist.find(destinationId)==dist.end()){
        result.found= false;
        return result;
    }
    else{
        result.found= true;
        result.totalCost = dist[destinationId];
    }

    string current= destinationId;
    while(current != originId){
        auto [prevAirport, flightId] = previous[current];
        result.path.push_back(current);
        result.flightIds.push_back(flightId);
        current= prevAirport;
    }
    result.path.push_back(originId);
    reverse(result.path.begin(), result.path.end());
    reverse(result.flightIds.begin(), result.flightIds.end());


    auto endTime = chrono::high_resolution_clock::now();
    result.nodesExplored = nodesExplored;
    result.runtimeMs = chrono::duration<double, milli>(endTime - startTime).count();

    return result;
}
// Implement dijkstra() here. See the TODO comments in include/dijkstra.h
// for the exact requirements and test cases to check against.
//
// Skeleton to get you started (fill in the logic, don't just copy-paste this
// without understanding each line — walk through it against docs/graph_theory_and_dijkstra_notes.md):
//
// PathResult dijkstra(const Graph& graph, const std::string& originId,
//                      const std::string& destinationId, WeightType weightType) {
//     PathResult result;
//     auto start = std::chrono::high_resolution_clock::now();
//
//     // cost so far to reach each airport
//     std::unordered_map<std::string, double> dist;
//     // how we reached each airport: (previous airport_id, flight_id used)
//     std::unordered_map<std::string, std::pair<std::string, std::string>> previous;
//     // min-heap of (cost, airport_id) - smallest cost popped first
//     std::priority_queue<...> pq;
//
//     // TODO: initialize dist[originId] = 0, push (0, originId) into pq
//     // TODO: main loop - pop smallest, skip if already finalized,
//     //       relax all outgoing edges, push updates, track nodesExplored
//     // TODO: once destinationId is popped (or pq empties), stop
//     // TODO: reconstruct path by walking `previous` backwards from destination
//     // TODO: fill in result.runtimeMs using `start` and the current time
//
//     return result;
// }
