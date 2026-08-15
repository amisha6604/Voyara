import json
import random

random.seed(42)

# Direct flight legs. Times are local to origin/destination per airports.json timezones.
# Deliberately includes: multi-edges (same route, diff airline/price/time),
# an overnight flight (day rollover), a tight-connection trap, and enough
# international legs to support the Varanasi->Delhi->Dubai->Paris->Amsterdam->Varanasi TSP example.

flights = []
fid = 1
def add(origin, dest, dep, arr, dur, price, airline, date="2026-09-10"):
    global fid
    flights.append({
        "flight_id": f"FL{fid:04d}",
        "airline": airline,
        "origin": origin,
        "destination": dest,
        "departure": f"{date}T{dep}:00",
        "arrival": f"{date}T{arr}:00" if arr > dep else f"2026-09-11T{arr}:00",  # crude overnight flag
        "duration_minutes": dur,
        "price_inr": price,
        "stops": 0,
        "date": date,
        "source": "seed",
        "retrieved_at": "2026-08-15T00:00:00Z"
    })
    fid += 1

# --- Domestic hub-and-spoke, India-first ---
add("VNS", "DEL", "08:00", "09:30", 90, 4200, "IndiGo")
add("VNS", "DEL", "12:00", "13:30", 90, 3900, "Air India")     # multi-edge: same route, diff price/time
add("VNS", "BOM", "07:15", "09:15", 120, 5600, "IndiGo")
add("DEL", "BOM", "10:00", "12:00", 120, 3800, "IndiGo")
add("DEL", "BOM", "10:00", "12:05", 125, 4100, "Vistara")      # multi-edge: same dep time, diff airline/price (equal-cost-ish path test)
add("DEL", "BLR", "09:00", "11:45", 165, 4700, "Air India")
add("BOM", "BLR", "13:00", "14:30", 90, 3600, "IndiGo")
add("DEL", "GOI", "06:30", "09:00", 150, 5200, "SpiceJet")
add("BOM", "GOI", "15:00", "16:10", 70, 2900, "IndiGo")
add("DEL", "COK", "11:00", "14:00", 180, 6100, "Air India")
add("BLR", "COK", "08:00", "09:20", 80, 2800, "IndiGo")
add("DEL", "HYD", "07:00", "09:15", 135, 4300, "Vistara")
add("BOM", "HYD", "18:00", "19:20", 80, 3100, "IndiGo")
add("DEL", "CCU", "06:00", "08:20", 140, 4500, "IndiGo")
add("DEL", "IXC", "08:00", "09:00", 60, 2600, "Air India")

# --- Tight-connection trap for time-dependent routing (Section 11 example) ---
# VNS->DEL arrives 12:00. Min connection buffer = 45 min.
add("VNS", "DEL", "10:30", "12:00", 90, 4000, "IndiGo")
add("DEL", "DXB", "12:20", "14:20", 240, 21000, "Emirates")    # INFEASIBLE: only 20 min buffer
add("DEL", "DXB", "13:10", "15:10", 240, 19500, "Air India")   # FEASIBLE: 70 min buffer
add("DEL", "DXB", "23:30", "01:45", 255, 18000, "IndiGo")      # overnight flight, day rollover

# --- International legs for multi-city TSP example: VNS-DEL-DXB-CDG-AMS-VNS ---
add("BOM", "DXB", "02:00", "04:15", 195, 20500, "Emirates")
add("BLR", "DXB", "23:00", "01:30", 210, 22000, "Emirates")
add("DXB", "CDG", "09:00", "14:00", 420, 34000, "Emirates")
add("DXB", "CDG", "10:30", "15:45", 435, 31500, "Air France")  # multi-edge
add("DXB", "SIN", "16:00", "23:30", 450, 26000, "Singapore Airlines")
add("DXB", "BKK", "17:00", "22:30", 330, 21000, "Thai Airways")
add("CDG", "AMS", "16:30", "17:45", 75, 8500, "Air France")
add("CDG", "AMS", "19:00", "20:15", 75, 7900, "KLM")           # multi-edge
add("AMS", "DEL", "22:00", "10:30", 555, 42000, "KLM")         # overnight, long-haul return
add("AMS", "BOM", "23:15", "12:00", 585, 44500, "KLM")
add("SIN", "DEL", "08:00", "12:45", 345, 28000, "Singapore Airlines")
add("BKK", "DEL", "09:30", "13:00", 270, 19000, "Thai Airways")

# --- Return/reverse legs so the graph isn't one-directional ---
add("DEL", "VNS", "18:00", "19:30", 90, 4100, "IndiGo")
add("BOM", "VNS", "20:00", "22:00", 120, 5400, "IndiGo")
add("BLR", "DEL", "19:00", "21:45", 165, 4600, "Air India")
add("GOI", "DEL", "17:00", "19:30", 150, 5100, "SpiceJet")
add("HYD", "DEL", "20:00", "22:15", 135, 4200, "Vistara")

with open("/home/claude/travel-optimizer/data/flights.json", "w") as f:
    json.dump(flights, f, indent=2)

print(f"Generated {len(flights)} flight legs -> data/flights.json")
