#include "scorer.h"

ScoreBreakdown calculateScore(
    const PathResult& route,
    const PathResult& cheapestReference,
    const PathResult& fastestReference,
    const PathResult& fewestStopsReference,
    const ScoringWeights& weights
) {
    ScoreBreakdown result;

    if (!route.found) {
        return result;
    }

    // ------------------------------------------------------------
    // Cost score
    //
    // Lower cost is better.
    // Cheapest route gets a score of 1.0.
    // ------------------------------------------------------------

    if (route.totalCost > 0 &&
        cheapestReference.totalCost > 0) {

        result.costScore =
            cheapestReference.totalCost /
            route.totalCost;
    }


    // ------------------------------------------------------------
    // Time score
    //
    // Lower elapsed travel time is better.
    // Fastest route gets a score of 1.0.
    // ------------------------------------------------------------

    if (route.totalTravelTimeMinutes > 0 &&
        fastestReference.totalTravelTimeMinutes > 0) {

        result.timeScore =
            static_cast<double>(
                fastestReference.totalTravelTimeMinutes
            ) /
            static_cast<double>(
                route.totalTravelTimeMinutes
            );
    }


    // ------------------------------------------------------------
    // Stops score
    //
    // Fewer stops are better.
    //
    // +1 prevents division by zero for direct flights.
    // Fewest-stop route gets a score of 1.0.
    // ------------------------------------------------------------

    if (route.stops >= 0 &&
        fewestStopsReference.stops >= 0) {

        result.stopsScore =
            static_cast<double>(
                fewestStopsReference.stops + 1
            ) /
            static_cast<double>(
                route.stops + 1
            );
    }


    // ------------------------------------------------------------
    // Weighted score
    // ------------------------------------------------------------

    result.totalScore =
        weights.costWeight * result.costScore +
        weights.timeWeight * result.timeScore +
        weights.stopsWeight * result.stopsScore;

    return result;
}