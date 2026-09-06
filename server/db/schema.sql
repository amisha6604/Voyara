-- Travel Optimization Engine — PostgreSQL schema
-- Implements v2 spec Section 6. Comments explain the "why" so you can defend
-- these choices in an interview, not just recite that they exist.

-- ============================================================
-- Reference/dimension tables
-- ============================================================

CREATE TABLE cities (
    city_id     SERIAL PRIMARY KEY,
    name        VARCHAR(100) NOT NULL,
    country     VARCHAR(100) NOT NULL,
    timezone    VARCHAR(50) NOT NULL
);
-- Normalized out from airports/hotels because multiple airports and multiple
-- hotels reference the same city. Without this table, "Delhi" would be a
-- repeated string across dozens of rows — a classic 2NF violation (city name
-- and country depend on city, not on the airport/hotel row itself).

CREATE TABLE airlines (
    airline_id  SERIAL PRIMARY KEY,
    name        VARCHAR(100) NOT NULL,
    code        VARCHAR(10)
);

CREATE TABLE data_sources (
    source_id       SERIAL PRIMARY KEY,
    provider_name   VARCHAR(100) NOT NULL,   -- e.g. 'seed', 'duffel', 'manual'
    source_type     VARCHAR(30) NOT NULL,     -- 'seeded' | 'cached' | 'live' | 'estimated'
    freshness_policy VARCHAR(100)             -- human-readable, e.g. 'static, no expiry'
);
-- Exists so every flight/hotel row can point at WHERE it came from and how
-- fresh it's allowed to be treated as. Spec Section 7 is explicit: the UI
-- must be able to distinguish seeded/cached/estimated/live data, and that's
-- only possible if provenance is stored per-record, not assumed globally.

-- ============================================================
-- Core entities
-- ============================================================

CREATE TABLE airports (
    airport_id  SERIAL PRIMARY KEY,
    iata        CHAR(3) NOT NULL UNIQUE,
    city_id     INTEGER NOT NULL REFERENCES cities(city_id),
    latitude    DECIMAL(9,6) NOT NULL,
    longitude   DECIMAL(9,6) NOT NULL,
    timezone    VARCHAR(50) NOT NULL
);

CREATE TABLE flights (
    flight_id       SERIAL PRIMARY KEY,
    airline_id      INTEGER NOT NULL REFERENCES airlines(airline_id),
    origin_airport  INTEGER NOT NULL REFERENCES airports(airport_id),
    destination_airport INTEGER NOT NULL REFERENCES airports(airport_id),
    departure_at    TIMESTAMP NOT NULL,   -- local to origin airport's timezone
    arrival_at      TIMESTAMP NOT NULL,   -- local to destination airport's timezone
    duration_minutes INTEGER NOT NULL,
    price_inr       INTEGER NOT NULL,
    source_id       INTEGER NOT NULL REFERENCES data_sources(source_id),
    retrieved_at    TIMESTAMP NOT NULL DEFAULT now(),
    status          VARCHAR(20) NOT NULL DEFAULT 'active'  -- active | cancelled | unavailable
);

CREATE TABLE hotels (
    hotel_id        SERIAL PRIMARY KEY,
    city_id         INTEGER NOT NULL REFERENCES cities(city_id),
    name            VARCHAR(150) NOT NULL,
    price_per_night_inr INTEGER NOT NULL,
    rating          DECIMAL(2,1),
    latitude        DECIMAL(9,6),
    longitude       DECIMAL(9,6),
    category        VARCHAR(20),   -- budget | mid | luxury
    source_id       INTEGER NOT NULL REFERENCES data_sources(source_id),
    retrieved_at    TIMESTAMP NOT NULL DEFAULT now()
);

-- ============================================================
-- Query / result entities
-- ============================================================

CREATE TABLE searches (
    search_id       SERIAL PRIMARY KEY,
    origin          CHAR(3) NOT NULL,
    destination     CHAR(3),               -- nullable: destination-discovery is a later feature
    start_date      DATE NOT NULL,
    duration_days   INTEGER,
    travellers      INTEGER NOT NULL DEFAULT 1,
    budget_inr      INTEGER,
    budget_is_hard  BOOLEAN NOT NULL DEFAULT false,
    preferences     JSONB,                 -- raw preference payload (priority mode, max layovers, etc.)
    created_at      TIMESTAMP NOT NULL DEFAULT now()
);
-- preferences is JSONB, not a pile of extra columns. Reason: the exact set of
-- preference fields is still evolving (spec Section 3 lists a dozen, more may
-- be added). JSONB avoids a schema migration every time a new soft-preference
-- field is added, while still being indexable/queryable if needed later
-- (this is the deliberate, justified use of JSONB the spec allows — not an
-- excuse to go document-store for everything).

CREATE TABLE itineraries (
    itinerary_id    SERIAL PRIMARY KEY,
    search_id       INTEGER NOT NULL REFERENCES searches(search_id),
    total_cost_inr  INTEGER NOT NULL,
    total_duration_minutes INTEGER NOT NULL,
    stops           INTEGER NOT NULL,
    score           DECIMAL(10,4),
    score_breakdown JSONB,   -- {"cost": 0.4, "time": 0.3, "stops": 0.1, ...} — per-objective contribution
    mode            VARCHAR(20),   -- cheapest | fastest | balanced | comfort | custom
    created_at      TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE itinerary_legs (
    itinerary_id    INTEGER NOT NULL REFERENCES itineraries(itinerary_id),
    flight_id       INTEGER NOT NULL REFERENCES flights(flight_id),
    leg_number      INTEGER NOT NULL,
    connection_minutes INTEGER,   -- buffer before this leg, null for the first leg
    PRIMARY KEY (itinerary_id, leg_number)
);

CREATE TABLE itinerary_hotels (
    itinerary_id    INTEGER NOT NULL REFERENCES itineraries(itinerary_id),
    hotel_id        INTEGER NOT NULL REFERENCES hotels(hotel_id),
    check_in        DATE NOT NULL,
    check_out       DATE NOT NULL,
    nights          INTEGER NOT NULL,
    total_cost_inr  INTEGER NOT NULL,
    PRIMARY KEY (itinerary_id, hotel_id)
);

-- ============================================================
-- Benchmarking (spec Section 18 — mandatory)
-- ============================================================

CREATE TABLE algorithm_runs (
    run_id          SERIAL PRIMARY KEY,
    algorithm       VARCHAR(50) NOT NULL,   -- 'dijkstra' | 'astar' | 'bitmask_dp' | 'greedy_tsp'
    dataset_size    INTEGER NOT NULL,       -- e.g. number of nodes/edges used
    runtime_ms      DECIMAL(10,3) NOT NULL,
    nodes_explored  INTEGER,
    solution_cost   DECIMAL(12,2),
    exact_or_heuristic VARCHAR(10) NOT NULL, -- 'exact' | 'heuristic'
    config          JSONB,                   -- any run-specific parameters worth recording
    created_at      TIMESTAMP NOT NULL DEFAULT now()
);
-- This table is what turns "we implemented Dijkstra and A*" into an
-- evidence-backed resume claim. Every benchmark run in Phase 2 writes here;
-- the final benchmark table in Phase 5 is a query against this, not a
-- hand-typed markdown table.

-- ============================================================
-- Indexes — every one has a query reason, per spec Section 6 rule
-- ============================================================

CREATE INDEX idx_airports_iata ON airports(iata);
-- Every graph lookup starts from an IATA code (e.g. "VNS") — this is the
-- single most frequent lookup in the whole system.

CREATE INDEX idx_flights_origin_dest_dep ON flights(origin_airport, destination_airport, departure_at);
-- Supports "find flights from X to Y around this date" — the core query
-- the graph-loading step and any live search will run constantly.

CREATE INDEX idx_flights_departure ON flights(departure_at);
-- Supports date-range queries independent of route (e.g. "what's flying out
-- of DEL tomorrow") and time-dependent routing's need to filter by time.

CREATE INDEX idx_hotels_city ON hotels(city_id);
-- Every hotel search is scoped to a city.

-- Deliberately NOT indexed yet: price/rating columns. No query pattern in
-- Phase 1-2 filters by price or rating alone without a city/route context
-- first — adding an index without a justifying query is exactly what the
-- spec's indexing rule warns against. Revisit if/when the API adds
-- price-range filtering as a first-class query.
