# Travel Optimization Engine

Algorithm-first, personalized travel optimization platform. Not a booking clone —
takes flight/hotel data + user preferences (money/time/comfort/custom) and produces
ranked, explainable itineraries via a C++ graph/optimization engine.

## Status
Actively being built. See `docs/decisions_log.md` for the running architecture and
scope decisions, and `docs/data_model.md` for the schema.

## Stack
- C++ optimization engine (Dijkstra, A*, bitmask DP for multi-city TSP)
- Node.js/Express API (talks to the C++ engine via subprocess)
- React + Vite + Tailwind frontend
- Seeded JSON data (India-first + intl hubs), real API integration deferred

## Structure
- `data/` — seed data (airports, flights, hotels)
- `engine/` — C++ optimization engine
- `server/` — Node/Express API
- `client/` — React frontend
- `docs/` — architecture, decisions, data model, benchmarks
