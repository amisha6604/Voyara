# Domain / Data Model — v1

## Why raw attributes, not a precomputed score

Every flight/hotel record stores **raw fields only** (price, duration, rating, distance).
No field stores a pre-baked "score." The objective function (cheapest / fastest /
balanced / comfort / custom) is applied **at query time**, in the C++ engine, against
the same underlying graph. This is what makes "different users get different optimal
answers from the same data" possible — if we baked in a score at ingestion, we'd need
to re-fetch or re-store data per objective, which defeats the point of a multi-objective
optimizer. (Spec §9.3, §16.1)

## airports.json

| Field | Type | Notes |
|---|---|---|
| airport_id | string | Primary key, same as IATA here |
| iata | string | 3-letter code |
| city, country | string | |
| latitude, longitude | float | Used for A* heuristic (great-circle distance) |
| timezone | string | IANA tz — required for correct connection-time math across zones |

**Edge case embedded:** `IXZ` (Port Blair) has no flights referencing it anywhere in
`flights.json`. This is the disconnected-node test case (Spec §9.4, §24.1) — Dijkstra/A*
must terminate cleanly and report "no path" rather than erroring.

## flights.json

| Field | Type | Notes |
|---|---|---|
| flight_id | string | Primary key |
| airline | string | |
| origin, destination | string | Airport IDs — these are the graph edges |
| departure, arrival | ISO 8601 local datetime | Local to each airport's timezone |
| duration_minutes | int | |
| price_inr | int | |
| stops | int | Always 0 here — every record is a **direct leg**. Multi-stop itineraries are *built* by the graph engine chaining direct legs, not stored pre-assembled. |
| date, source, retrieved_at | string | Provenance metadata (Spec §7.4, §26) |

**Edge cases embedded, all intentional:**
- **Multi-edge routes** — e.g. two `VNS→DEL` flights, two `DEL→BOM` flights, two
  `DXB→CDG` flights, two `CDG→AMS` flights — same route pair, different airline/price/time.
  Tests that the graph correctly treats parallel edges as distinct options, not duplicates.
- **Tight-connection trap** — `VNS→DEL` arrives 12:00. One `DEL→DXB` departs 12:20
  (only 20 min buffer → **infeasible** if min connection = 45 min). Another `DEL→DXB`
  departs 13:10 (70 min buffer → **feasible**). This is the exact example from Spec §11.
- **Overnight/day-rollover flights** — `DEL→DXB` at 23:30 arriving 01:45 next day,
  and both AMS→India long-haul return legs. Tests date-rollover handling in time math.
- **TSP-ready international mesh** — enough legs exist to route
  `VNS → DEL → DXB → CDG → AMS → (back to India)`, matching the multi-city example
  in Spec §12.1 almost exactly, for testing the bitmask DP.

## hotels.json

| Field | Type | Notes |
|---|---|---|
| hotel_id | string | Primary key |
| city | string | Joins to flight destination city |
| name | string | |
| price_per_night_inr | int | |
| rating | float | 1–5 |
| distance_from_airport_km | float | Proxy for "hotel location" until real geocoding is added |
| category | string | budget / mid / luxury |
| amenities | list[string] | |
| source, retrieved_at | string | Provenance |

Three price/rating tiers per city (budget/mid/luxury) so the joint flight+hotel
scoring (Spec §15) has a real spread to optimize over — a cheap flight + expensive
"only option" hotel vs. a pricier flight + cheap hotel is the whole point of Spec §15's
worked example, and this dataset can actually reproduce that tradeoff.

## Deliberately NOT modeled yet (Tier 2 / later phases)

- Ground transport as a separate graph
- Live/real API data (provider TBD — Amadeus self-service is defunct as of July 2026)
- Seat-level availability
- User accounts / saved searches

These are absent by decision, not oversight — see `decisions_log.md`.
