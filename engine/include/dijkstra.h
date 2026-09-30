#pragma once
#include <string>
#include <vector>
#include "graph.h"

// What you're optimizing the route by.
// PRICE     -> minimize total flight price
// DURATION  -> minimize total flight duration
enum class WeightType {
    PRICE,
    DURATION
};

// Hard constraints supplied by the user.
// A route that violates any enabled constraint is rejected.
struct RouteConstraints {
    double maxBudget = -1.0;
    int maxStops = -1;
    int maxTravelTimeMinutes = -1;
};

struct PathResult {
    bool found = false;
    double totalCost = 0.0;

    std::vector<std::string> path;
    std::vector<std::string> flightIds;

    // Actual elapsed travel time from first departure
    // to final arrival.
    long long totalTravelTimeMinutes = 0;

    // Number of connections/stops.
    int stops = 0;

    // Benchmarking fields.
    long long nodesExplored = 0;
    double runtimeMs = 0.0;
};

PathResult dijkstra(
    const Graph& graph,
    const std::string& originId,
    const std::string& destinationId,
    WeightType weightType,
    const RouteConstraints& constraints
);