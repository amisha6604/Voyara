#include <cassert>
#include <cmath>
#include <iostream>

#include "scorer.h"

int main() {

    PathResult cheapest;
    cheapest.found = true;
    cheapest.totalCost = 10000;
    cheapest.totalTravelTimeMinutes = 600;
    cheapest.stops = 2;


    PathResult fastest;
    fastest.found = true;
    fastest.totalCost = 20000;
    fastest.totalTravelTimeMinutes = 300;
    fastest.stops = 1;


    PathResult fewestStops;
    fewestStops.found = true;
    fewestStops.totalCost = 15000;
    fewestStops.totalTravelTimeMinutes = 500;
    fewestStops.stops = 0;


    PathResult current;
    current.found = true;
    current.totalCost = 20000;
    current.totalTravelTimeMinutes = 600;
    current.stops = 1;


    ScoringWeights weights;


    ScoreBreakdown score =
        calculateScore(
            current,
            cheapest,
            fastest,
            fewestStops,
            weights
        );


    std::cout
        << "Cost score: "
        << score.costScore
        << '\n';

    std::cout
        << "Time score: "
        << score.timeScore
        << '\n';

    std::cout
        << "Stops score: "
        << score.stopsScore
        << '\n';

    std::cout
        << "Total score: "
        << score.totalScore
        << '\n';


    assert(
        std::abs(score.costScore - 0.5) < 1e-9
    );

    assert(
        std::abs(score.timeScore - 0.5) < 1e-9
    );

    assert(
        std::abs(score.stopsScore - 0.5) < 1e-9
    );

    assert(
        std::abs(score.totalScore - 0.5) < 1e-9
    );


    std::cout
        << "Scorer test passed!"
        << '\n';

    return 0;
}