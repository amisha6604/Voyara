#pragma once

#include "dijkstra.h"

struct ScoringWeights {

    double costWeight = 0.5;

    double timeWeight = 0.3;

    double stopsWeight = 0.2;

};

struct ScoreBreakdown {

    double costScore = 0.0;

    double timeScore = 0.0;

    double stopsScore = 0.0;

    double totalScore = 0.0;

};

ScoreBreakdown calculateScore(

    const PathResult& route,

    const PathResult& cheapestReference,

    const PathResult& fastestReference,

    const PathResult& fewestStopsReference,

    const ScoringWeights& weights

);