import json

hotels = []
hid = 1
def add(city, name, price, rating, dist_km, category, amenities):
    global hid
    hotels.append({
        "hotel_id": f"HT{hid:04d}",
        "city": city,
        "name": name,
        "price_per_night_inr": price,
        "rating": rating,
        "distance_from_airport_km": dist_km,
        "category": category,
        "amenities": amenities,
        "source": "seed",
        "retrieved_at": "2026-08-15T00:00:00Z"
    })
    hid += 1

# 3 tiers per major city: budget / mid / premium, so scoring has real spread to work with
plan = [
    ("Delhi", [
        ("Airport Transit Inn", 1800, 3.4, 2.1, "budget", ["wifi"]),
        ("Delhi Central Suites", 4200, 4.1, 8.5, "mid", ["wifi","breakfast","gym"]),
        ("The Grand Capital", 11500, 4.7, 12.0, "luxury", ["wifi","breakfast","gym","spa","pool"]),
    ]),
    ("Mumbai", [
        ("Andheri Stopover", 2000, 3.6, 3.0, "budget", ["wifi"]),
        ("Bandra Bay Hotel", 5200, 4.3, 9.2, "mid", ["wifi","breakfast","gym"]),
        ("Marine Vista Palace", 14000, 4.8, 14.5, "luxury", ["wifi","breakfast","gym","spa","pool"]),
    ]),
    ("Varanasi", [
        ("Ganga View Lodge", 1500, 3.8, 6.0, "budget", ["wifi"]),
        ("Kashi Heritage Hotel", 3600, 4.2, 4.5, "mid", ["wifi","breakfast"]),
        ("Riverside Regency", 9000, 4.6, 5.0, "luxury", ["wifi","breakfast","spa","pool"]),
    ]),
    ("Goa", [
        ("Beachside Backpackers", 1700, 3.9, 5.0, "budget", ["wifi"]),
        ("Candolim Comfort Inn", 4800, 4.4, 7.0, "mid", ["wifi","breakfast","pool"]),
        ("Palm Grove Resort", 13500, 4.7, 9.5, "luxury", ["wifi","breakfast","spa","pool","beach access"]),
    ]),
    ("Bengaluru", [
        ("Whitefield Budget Stay", 1900, 3.5, 15.0, "budget", ["wifi"]),
        ("MG Road Business Hotel", 4600, 4.2, 10.0, "mid", ["wifi","breakfast","gym"]),
        ("Garden City Grand", 10800, 4.6, 12.5, "luxury", ["wifi","breakfast","gym","spa","pool"]),
    ]),
    ("Kochi", [
        ("Backwater Budget Inn", 1600, 3.7, 4.0, "budget", ["wifi"]),
        ("Fort Kochi Suites", 4000, 4.3, 8.0, "mid", ["wifi","breakfast"]),
        ("Malabar Coast Resort", 9800, 4.6, 6.5, "luxury", ["wifi","breakfast","spa","pool"]),
    ]),
    ("Hyderabad", [
        ("HITEC Transit Rooms", 1750, 3.5, 5.0, "budget", ["wifi"]),
        ("Charminar Comfort", 4100, 4.1, 9.0, "mid", ["wifi","breakfast","gym"]),
        ("Nizam Grand Palace", 10200, 4.6, 11.0, "luxury", ["wifi","breakfast","gym","spa","pool"]),
    ]),
    ("Kolkata", [
        ("Howrah Budget Stay", 1650, 3.4, 4.5, "budget", ["wifi"]),
        ("Park Street Residency", 3900, 4.0, 7.5, "mid", ["wifi","breakfast"]),
        ("Victoria Grand Hotel", 9500, 4.5, 10.0, "luxury", ["wifi","breakfast","spa","pool"]),
    ]),
    ("Chandigarh", [
        ("Sector 17 Inn", 1500, 3.6, 3.0, "budget", ["wifi"]),
        ("Rock Garden Hotel", 3500, 4.1, 6.0, "mid", ["wifi","breakfast"]),
    ]),
    ("Dubai", [
        ("Deira Budget Rooms", 5500, 3.6, 6.0, "budget", ["wifi"]),
        ("Downtown Comfort Suites", 12000, 4.3, 10.0, "mid", ["wifi","breakfast","gym"]),
        ("Burj View Palace", 32000, 4.8, 15.0, "luxury", ["wifi","breakfast","gym","spa","pool"]),
    ]),
    ("Singapore", [
        ("Chinatown Budget Stay", 6200, 3.7, 18.0, "budget", ["wifi"]),
        ("Marina Bay Comfort", 15500, 4.4, 20.0, "mid", ["wifi","breakfast","gym"]),
        ("Sands Grand Suites", 38000, 4.9, 21.0, "luxury", ["wifi","breakfast","gym","spa","pool"]),
    ]),
    ("Bangkok", [
        ("Sukhumvit Budget Inn", 3200, 3.5, 25.0, "budget", ["wifi"]),
        ("Riverside Comfort Hotel", 7800, 4.2, 30.0, "mid", ["wifi","breakfast","pool"]),
        ("Grand Palace Resort", 18000, 4.7, 28.0, "luxury", ["wifi","breakfast","spa","pool"]),
    ]),
    ("Paris", [
        ("Gare du Nord Budget", 9500, 3.6, 26.0, "budget", ["wifi"]),
        ("Le Marais Comfort", 21000, 4.3, 30.0, "mid", ["wifi","breakfast"]),
        ("Champs-Elysees Grand", 48000, 4.8, 33.0, "luxury", ["wifi","breakfast","spa","gym"]),
    ]),
    ("Amsterdam", [
        ("Canal Budget Stay", 8800, 3.7, 12.0, "budget", ["wifi"]),
        ("Jordaan Comfort Hotel", 19500, 4.4, 15.0, "mid", ["wifi","breakfast"]),
        ("Museumplein Grand", 42000, 4.7, 17.0, "luxury", ["wifi","breakfast","spa","gym"]),
    ]),
]

for city, tiers in plan:
    for name, price, rating, dist, cat, amen in tiers:
        add(city, name, price, rating, dist, cat, amen)

with open("/home/claude/travel-optimizer/data/hotels.json", "w") as f:
    json.dump(hotels, f, indent=2)

print(f"Generated {len(hotels)} hotels -> data/hotels.json")
