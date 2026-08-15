import json, random, math

random.seed(7)

# Synthetic large-scale graph purely for benchmarking Dijkstra vs A* (spec Section 25).
# Not meant to be realistic city names — meant to have real scale so timing differences
# between algorithms are actually measurable (a 15-node graph runs in microseconds
# either way and proves nothing).

N_AIRPORTS = 500       # nodes
AVG_DEGREE = 6          # avg outgoing flights per airport, keeps graph sparse & realistic

airports = []
for i in range(N_AIRPORTS):
    # scatter points roughly over a lat/lon box covering India + nearby international space
    lat = random.uniform(6.0, 55.0)
    lon = random.uniform(65.0, 105.0)
    airports.append({
        "airport_id": f"BX{i:04d}",
        "iata": f"BX{i:04d}",
        "city": f"City{i}",
        "country": "Synthetic",
        "latitude": round(lat, 4),
        "longitude": round(lon, 4),
        "timezone": "Asia/Kolkata"
    })

def haversine_km(a, b):
    R = 6371
    lat1, lon1, lat2, lon2 = map(math.radians, [a["latitude"], a["longitude"], b["latitude"], b["longitude"]])
    dlat, dlon = lat2 - lat1, lon2 - lon1
    h = math.sin(dlat/2)**2 + math.cos(lat1)*math.cos(lat2)*math.sin(dlon/2)**2
    return 2 * R * math.asin(math.sqrt(h))

flights = []
fid = 1
for i, origin in enumerate(airports):
    # connect each airport to AVG_DEGREE nearest-ish others (not pure nearest, some randomness)
    candidates = random.sample(airports, min(30, N_AIRPORTS - 1))
    candidates = [c for c in candidates if c["airport_id"] != origin["airport_id"]]
    candidates.sort(key=lambda c: haversine_km(origin, c))
    chosen = candidates[:AVG_DEGREE]
    for dest in chosen:
        dist = haversine_km(origin, dest)
        duration = max(30, int(dist / 12) + random.randint(-10, 20))  # rough speed model + noise
        price = max(1500, int(dist * random.uniform(4.5, 9.0)))
        dep_h = random.randint(0, 23)
        dep_m = random.choice([0, 15, 30, 45])
        flights.append({
            "flight_id": f"BXF{fid:06d}",
            "airline": "SynthAir",
            "origin": origin["airport_id"],
            "destination": dest["airport_id"],
            "departure": f"2026-09-10T{dep_h:02d}:{dep_m:02d}:00",
            "duration_minutes": duration,
            "price_inr": price,
            "stops": 0,
            "date": "2026-09-10",
            "source": "synthetic-benchmark",
            "retrieved_at": "2026-08-15T00:00:00Z"
        })
        fid += 1

with open("/home/claude/travel-optimizer/data/benchmark/airports_bench.json", "w") as f:
    json.dump(airports, f)
with open("/home/claude/travel-optimizer/data/benchmark/flights_bench.json", "w") as f:
    json.dump(flights, f)

print(f"Benchmark graph: {len(airports)} nodes, {len(flights)} edges -> data/benchmark/")
