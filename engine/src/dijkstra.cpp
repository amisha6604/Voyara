#include "dijkstra.h"
#include <queue>
#include <unordered_map>
#include <limits>
#include <chrono>
#include <algorithm>

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
