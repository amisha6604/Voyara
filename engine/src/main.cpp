#include <iostream>
#include <string>
#include <vector>

#include "graph.h"
#include "dijkstra.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

int main() {

    try {

        string input;
        getline(cin, input);

        json request = json::parse(input);

        string origin = request["origin"];
        string destination = request["destination"];

        string weightType =
            request.value("weightType", "PRICE");


        // --------------------------------------------------------
        // Parse optimization objective
        // --------------------------------------------------------

        WeightType weight;

        if (weightType == "PRICE") {
            weight = WeightType::PRICE;
        }
        else if (weightType == "DURATION") {
            weight = WeightType::DURATION;
        }
        else {

            cout << json({
                {"found", false},
                {"error",
                 "Invalid weightType. Use PRICE or DURATION."}
            }).dump() << endl;

            return 1;
        }


        // --------------------------------------------------------
        // Parse hard route constraints
        //
        // Missing constraint = -1 = no constraint
        // --------------------------------------------------------

        RouteConstraints constraints;

        if (request.contains("constraints") &&
            request["constraints"].is_object()) {

            const auto& c = request["constraints"];

            constraints.maxBudget =
                c.value("maxBudget", -1.0);

            constraints.maxStops =
                c.value("maxStops", -1);

            constraints.maxTravelTimeMinutes =
                c.value(
                    "maxTravelTimeMinutes",
                    -1
                );
        }


        // --------------------------------------------------------
        // Build graph from flight data supplied by Node/PostgreSQL
        // --------------------------------------------------------

        Graph g;

        if (!request.contains("flights") ||
            !request["flights"].is_array()) {

            cout << json({
                {"found", false},
                {"error", "Missing flights array."}
            }).dump() << endl;

            return 1;
        }


        for (const auto& flight :
             request["flights"]) {

            string flightId =
                flight["flight_id"];

            string flightOrigin =
                flight["origin"];

            string flightDestination =
                flight["destination"];

            double price =
                flight["price_inr"];

            double duration =
                flight["duration_minutes"];


            Edge edge;

            edge.flight_id =
                flightId;

            edge.destination =
                flightDestination;

            edge.airline =
                flight.value("airline", "");

            edge.departure =
                flight.value("departure", "");

            edge.arrival =
                flight.value("arrival", "");

            edge.price_inr =
                static_cast<int>(price);

            edge.duration_minutes =
                static_cast<int>(duration);


            g.addFlight(
                flightOrigin,
                edge
            );
        }


        // --------------------------------------------------------
        // Run constrained routing
        // --------------------------------------------------------

        auto result = dijkstra(
            g,
            origin,
            destination,
            weight,
            constraints
        );


        // --------------------------------------------------------
        // Build response
        // --------------------------------------------------------

        json response;

        response["found"] =
            result.found;

        response["nodesExplored"] =
            result.nodesExplored;

        response["runtimeMs"] =
            result.runtimeMs;


        if (result.found) {

            response["totalCost"] =
                result.totalCost;

            response["totalTravelTimeMinutes"] =
                result.totalTravelTimeMinutes;

            response["stops"] =
                result.stops;

            response["path"] =
                result.path;

            response["flightIds"] =
                result.flightIds;
        }


        cout << response.dump() << endl;

        return 0;

    }
    catch (const exception& e) {

        cout << json({
            {"found", false},
            {"error", e.what()}
        }).dump() << endl;

        return 1;
    }
}