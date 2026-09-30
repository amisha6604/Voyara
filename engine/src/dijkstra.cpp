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

    return era * 146097 + static_cast<long long>(dayOfEra);
}

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

        return days * 24 * 60 + hour * 60 + minute;
    }
    catch (...) {
        return -1;
    }
}

struct Label {
    string airport;

    double cost = 0.0;

    long long arrivalTime = 0;

    long long departureTime = 0;

    int stops = 0;

    int previousLabel = -1;

    string flightId;

    bool active = true;
};

bool dominates(const Label& a, const Label& b) {
    return
        a.cost <= b.cost &&
        a.arrivalTime <= b.arrivalTime &&
        a.stops <= b.stops;
}

} // namespace


PathResult dijkstra(
    const Graph& graph,
    const string& originId,
    const string& destinationId,
    WeightType weightType,
    const RouteConstraints& constraints
) {
    PathResult result;

    auto startTime = chrono::high_resolution_clock::now();

    vector<Label> labels;

    unordered_map<string, vector<int>> labelsAtAirport;

    using QueueEntry = pair<double, int>;

    priority_queue<
        QueueEntry,
        vector<QueueEntry>,
        greater<>
    > pq;


    // ------------------------------------------------------------
    // Start label
    // ------------------------------------------------------------

    Label startLabel;

    startLabel.airport = originId;

    startLabel.cost = 0.0;

    // Special value means:
    // there is no previous flight, so the first flight
    // does not need a connection-time check.
    startLabel.arrivalTime =
        numeric_limits<long long>::min();

    startLabel.departureTime =
        numeric_limits<long long>::min();

    startLabel.stops = 0;

    labels.push_back(startLabel);

    labelsAtAirport[originId].push_back(0);

    pq.push({0.0, 0});


    long long nodesExplored = 0;


    // ------------------------------------------------------------
    // Main multi-label Dijkstra
    // ------------------------------------------------------------

    while (!pq.empty()) {

        auto [currentCost, labelId] = pq.top();
        pq.pop();

        (void)currentCost;

        Label& current = labels[labelId];

        if (!current.active) {
            continue;
        }

        nodesExplored++;


        // --------------------------------------------------------
        // Destination reached
        // --------------------------------------------------------

        if (current.airport == destinationId) {

            result.found = true;

            result.totalCost = current.cost;

            result.stops = current.stops;

            if (current.arrivalTime !=
                numeric_limits<long long>::min()) {

                result.totalTravelTimeMinutes =
                    current.arrivalTime -
                    current.departureTime;
            }


            // Reconstruct route.
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


        // --------------------------------------------------------
        // Explore outgoing flights
        // --------------------------------------------------------

        for (const auto& edge :
             graph.getEdges(current.airport)) {

            long long departureTime =
                parseTimestamp(edge.departure);

            long long arrivalTime =
                parseTimestamp(edge.arrival);

            if (departureTime < 0 ||
                arrivalTime < 0) {
                continue;
            }


            // ----------------------------------------------------
            // Connection-time constraint
            // ----------------------------------------------------

            if (current.arrivalTime !=
                numeric_limits<long long>::min()) {

                long long earliestDeparture =
                    current.arrivalTime +
                    MIN_CONNECTION_MINUTES;

                if (departureTime < earliestDeparture) {
                    continue;
                }
            }


            // ----------------------------------------------------
            // Calculate cost
            // ----------------------------------------------------

            double edgeWeight =
                (weightType == WeightType::PRICE)
                    ? edge.price_inr
                    : edge.duration_minutes;

            double newCost =
                current.cost + edgeWeight;


            // ----------------------------------------------------
            // Number of stops
            //
            // n flights => n - 1 stops
            // ----------------------------------------------------

            int newStops = current.stops;

            if (current.previousLabel != -1) {
                newStops++;
            }

            // ----------------------------------------------------
            // HARD CONSTRAINT: maximum stops
            // ----------------------------------------------------

            if (constraints.maxStops >= 0 &&
                newStops > constraints.maxStops){

                continue;
            }


            // ----------------------------------------------------
            // HARD CONSTRAINT: budget
            // ----------------------------------------------------

            if (constraints.maxBudget >= 0 &&
                newCost > constraints.maxBudget) {

                continue;
            }


            // ----------------------------------------------------
            // Determine itinerary departure time.
            //
            // For the first flight this becomes the
            // starting departure time.
            // ----------------------------------------------------

            long long itineraryDepartureTime =
                current.departureTime;

            if (itineraryDepartureTime ==
                numeric_limits<long long>::min()) {

                itineraryDepartureTime =
                    departureTime;
            }


            // ----------------------------------------------------
            // HARD CONSTRAINT: maximum travel time
            //
            // This is elapsed time:
            //
            // first departure -> final arrival
            //
            // so waiting/layovers are included.
            // ----------------------------------------------------

            long long elapsedTravelTime =
                arrivalTime -
                itineraryDepartureTime;

            if (constraints.maxTravelTimeMinutes >= 0 &&
                elapsedTravelTime >
                    constraints.maxTravelTimeMinutes) {

                continue;
            }


            // ----------------------------------------------------
            // Candidate label
            // ----------------------------------------------------

            Label candidate;

            candidate.airport =
                edge.destination;

            candidate.cost =
                newCost;

            candidate.arrivalTime =
                arrivalTime;

            candidate.departureTime =
                itineraryDepartureTime;

            candidate.stops =
                newStops;

            candidate.previousLabel =
                labelId;

            candidate.flightId =
                edge.flight_id;


            // ----------------------------------------------------
            // Dominance check
            // ----------------------------------------------------

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


            // ----------------------------------------------------
            // Add candidate label
            // ----------------------------------------------------

            int newLabelId =
                static_cast<int>(labels.size());

            labels.push_back(candidate);


            // ----------------------------------------------------
            // Remove labels dominated by candidate
            // ----------------------------------------------------

            auto& destinationLabels =
                labelsAtAirport[edge.destination];

            for (int existingId :
                 destinationLabels) {

                Label& existing =
                    labels[existingId];

                if (existing.active &&
                    dominates(
                        labels[newLabelId],
                        existing
                    )) {

                    existing.active = false;
                }
            }


            destinationLabels.push_back(
                newLabelId
            );

            pq.push({
                newCost,
                newLabelId
            });
        }
    }


    // ------------------------------------------------------------
    // Benchmarking
    // ------------------------------------------------------------

    auto endTime =
        chrono::high_resolution_clock::now();

    result.nodesExplored =
        nodesExplored;

    result.runtimeMs =
        chrono::duration<double, milli>(
            endTime - startTime
        ).count();

    return result;
}