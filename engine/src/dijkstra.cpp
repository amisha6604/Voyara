#include "dijkstra.h"

#include <queue>
#include <unordered_map>
#include <vector>
#include <limits>
#include <chrono>
#include <algorithm>
#include <string>

using namespace std;

namespace {

constexpr long long MIN_CONNECTION_MINUTES = 60;

// Convert YYYY-MM-DD to a day number.
// This is timezone-independent and works for comparing local airport times.
long long daysFromCivil(int year, unsigned month, unsigned day) {

    year -= (month <= 2);

    const long long era =
        (year >= 0 ? year : year - 399) / 400;

    const unsigned yearOfEra =
        static_cast<unsigned>(year - era * 400);

    const unsigned monthPrime =
        month + (month > 2 ? -3 : 9);

    const unsigned dayOfYear =
        (153 * monthPrime + 2) / 5 + day - 1;

    const unsigned dayOfEra =
        yearOfEra * 365
        + yearOfEra / 4
        - yearOfEra / 100
        + dayOfYear;

    return era * 146097
         + static_cast<long long>(dayOfEra);
}


// Parse:
// YYYY-MM-DDTHH:MM:SS
//
// We deliberately do NOT apply a timezone.
// These timestamps represent local airport times.
long long parseTimestamp(const string& timestamp) {

    if (timestamp.size() < 19) {
        return -1;
    }

    try {

        int year = stoi(timestamp.substr(0, 4));
        int month = stoi(timestamp.substr(5, 2));
        int day = stoi(timestamp.substr(8, 2));
        int hour = stoi(timestamp.substr(11, 2));
        int minute = stoi(timestamp.substr(14, 2));
        

        long long days =
            daysFromCivil(
                year,
                static_cast<unsigned>(month),
                static_cast<unsigned>(day)
            );

        return days * 24 * 60
             + hour * 60
             + minute;

    } catch (...) {
        return -1;
    }
}


struct Label {

    string airport;

    double cost = 0.0;

    // Arrival time at this airport in minutes.
    long long arrivalTime = 0;

    // Previous label used for path reconstruction.
    int previousLabel = -1;

    // Flight used to reach this label.
    string flightId;

    // A label can become dominated by a better label.
    bool active = true;
};


// A label A dominates label B when A is:
// - no more expensive
// - no later
//
// If both are true, B can never produce a better continuation.
bool dominates(const Label& a, const Label& b) {

    return a.cost <= b.cost
        && a.arrivalTime <= b.arrivalTime;
}

} // namespace


PathResult dijkstra(
    const Graph& graph,
    const std::string& originId,
    const std::string& destinationId,
    WeightType weightType
) {

    PathResult result;

    auto startTime =
        chrono::high_resolution_clock::now();

    vector<Label> labels;

    // Labels grouped by airport.
    unordered_map<string, vector<int>> labelsAtAirport;

    /*
     * Priority queue:
     *
     * The first value is the optimization cost.
     *
     * PRICE    → accumulated ticket price
     * DURATION → accumulated flight duration
     */
    using QueueEntry = pair<double, int>;

    priority_queue<
        QueueEntry,
        vector<QueueEntry>,
        greater<>
    > pq;


    // Starting label.
    //
    // LLONG_MIN means:
    // "There is no previous flight, so connection time does not apply."
    Label startLabel;

    startLabel.airport = originId;
    startLabel.cost = 0.0;
    startLabel.arrivalTime =
        numeric_limits<long long>::min();

    labels.push_back(startLabel);

    labelsAtAirport[originId].push_back(0);

    pq.push({0.0, 0});


    long long nodesExplored = 0;


    while (!pq.empty()) {

        auto [currentCost, labelId] = pq.top();
        pq.pop();

        Label& current = labels[labelId];

        // This label was dominated by another label.
        if (!current.active) {
            continue;
        }

        nodesExplored++;


        // Because the priority queue is ordered by optimization cost,
        // the first active destination label is optimal for the
        // selected WeightType.
        if (current.airport == destinationId) {

            result.found = true;
            result.totalCost = current.cost;


            // Reconstruct airport path and flight path.
            vector<string> reversedPath;
            vector<string> reversedFlights;

            int currentId = labelId;

            while (currentId != -1) {

                const Label& label = labels[currentId];

                reversedPath.push_back(label.airport);

                if (label.previousLabel != -1) {
                    reversedFlights.push_back(label.flightId);
                }

                currentId = label.previousLabel;
            }

            reverse(
                reversedPath.begin(),
                reversedPath.end()
            );

            reverse(
                reversedFlights.begin(),
                reversedFlights.end()
            );

            result.path = reversedPath;
            result.flightIds = reversedFlights;

            break;
        }


        // Explore all flights leaving the current airport.
        for (const auto& edge :
             graph.getEdges(current.airport)) {


            long long departureTime =
                parseTimestamp(edge.departure);

            long long arrivalTime =
                parseTimestamp(edge.arrival);


            // Invalid timestamp → cannot safely use this flight.
            if (departureTime < 0 || arrivalTime < 0) {
                continue;
            }


            /*
             * Connection feasibility.
             *
             * For the starting airport:
             *
             *     no previous arrival
             *     → any departure is allowed
             *
             * For connecting flights:
             *
             *     departure >= previous arrival + 60 minutes
             */
            if (
                current.arrivalTime !=
                numeric_limits<long long>::min()
            ) {

                long long earliestDeparture =
                    current.arrivalTime
                    + MIN_CONNECTION_MINUTES;

                if (departureTime < earliestDeparture) {
                    continue;
                }
            }


            double edgeWeight =
                (weightType == WeightType::PRICE)
                    ? edge.price_inr
                    : edge.duration_minutes;


            double newCost =
                current.cost + edgeWeight;


            Label candidate;

            candidate.airport = edge.destination;
            candidate.cost = newCost;
            candidate.arrivalTime = arrivalTime;
            candidate.previousLabel = labelId;
            candidate.flightId = edge.flight_id;


            /*
             * Check whether an existing label at the same airport
             * already dominates this candidate.
             */
            bool candidateDominated = false;

            for (int existingId :
                 labelsAtAirport[edge.destination]) {

                const Label& existing =
                    labels[existingId];

                if (!existing.active) {
                    continue;
                }

                if (dominates(existing, candidate)) {
                    candidateDominated = true;
                    break;
                }
            }

            if (candidateDominated) {
                continue;
            }


            /*
             * Candidate is useful.
             *
             * It may dominate some older labels.
             */
            int newLabelId =
                static_cast<int>(labels.size());

            labels.push_back(candidate);

            auto& destinationLabels =
                labelsAtAirport[edge.destination];


            for (int existingId :
                 destinationLabels) {

                Label& existing =
                    labels[existingId];

                if (
                    existing.active
                    && dominates(
                        labels[newLabelId],
                        existing
                    )
                ) {
                    existing.active = false;
                }
            }


            destinationLabels.push_back(newLabelId);

            pq.push({
                newCost,
                newLabelId
            });
        }
    }


    auto endTime =
        chrono::high_resolution_clock::now();

    result.nodesExplored = nodesExplored;

    result.runtimeMs =
        chrono::duration<double, milli>(
            endTime - startTime
        ).count();


    return result;
}