// server/db/seed_loader.js
//
// Loads data/airports.json, flights.json, hotels.json into PostgreSQL.
// Run once after creating the schema: node db/seed_loader.js
//
// Requires: npm install pg
// Requires env vars (or edit the pool config directly): PGHOST, PGUSER,
// PGPASSWORD, PGDATABASE, PGPORT — or just run with defaults if your local
// Postgres uses the standard setup (see docs/postgres_setup.md).

const { Pool } = require('pg');
const fs = require('fs');
const path = require('path');

const pool = new Pool(); // reads PG* env vars automatically

const DATA_DIR = path.join(__dirname, '..', '..', 'data');

function loadJSON(filename) {
  return JSON.parse(fs.readFileSync(path.join(DATA_DIR, filename), 'utf-8'));
}

async function main() {
  const client = await pool.connect();
  try {
    await client.query('BEGIN');

    const airports = loadJSON('airports.json');
    const flights = loadJSON('flights.json');
    const hotels = loadJSON('hotels.json');

    // --- data_sources: one row for 'seed' provenance ---
    const sourceRes = await client.query(
      `INSERT INTO data_sources (provider_name, source_type, freshness_policy)
       VALUES ('seed', 'seeded', 'static, no expiry') RETURNING source_id`
    );
    const seedSourceId = sourceRes.rows[0].source_id;

    // --- cities: unique (name, country) pairs from airports.json ---
    const cityMap = new Map(); // key: `${city}|${country}` -> city_id
    for (const a of airports) {
      const key = `${a.city}|${a.country}`;
      if (!cityMap.has(key)) {
        const res = await client.query(
          `INSERT INTO cities (name, country, timezone) VALUES ($1, $2, $3) RETURNING city_id`,
          [a.city, a.country, a.timezone]
        );
        cityMap.set(key, res.rows[0].city_id);
      }
    }
    console.log(`Inserted ${cityMap.size} cities`);

    // --- airports ---
    const airportIdMap = new Map(); // iata -> airport_id
    for (const a of airports) {
      const cityId = cityMap.get(`${a.city}|${a.country}`);
      const res = await client.query(
        `INSERT INTO airports (iata, city_id, latitude, longitude, timezone)
         VALUES ($1, $2, $3, $4, $5) RETURNING airport_id`,
        [a.iata, cityId, a.latitude, a.longitude, a.timezone]
      );
      airportIdMap.set(a.iata, res.rows[0].airport_id);
    }
    console.log(`Inserted ${airportIdMap.size} airports`);

    // --- airlines: unique names from flights.json ---
    const airlineMap = new Map(); // name -> airline_id
    const uniqueAirlines = [...new Set(flights.map(f => f.airline))];
    for (const name of uniqueAirlines) {
      const res = await client.query(
        `INSERT INTO airlines (name) VALUES ($1) RETURNING airline_id`,
        [name]
      );
      airlineMap.set(name, res.rows[0].airline_id);
    }
    console.log(`Inserted ${airlineMap.size} airlines`);

    // --- flights ---
    let flightCount = 0;
    for (const f of flights) {
      const originId = airportIdMap.get(f.origin);
      const destId = airportIdMap.get(f.destination);
      if (!originId || !destId) {
        console.warn(`Skipping flight ${f.flight_id}: unknown airport ${f.origin} or ${f.destination}`);
        continue;
      }
      await client.query(
        `INSERT INTO flights (airline_id, origin_airport, destination_airport,
           departure_at, arrival_at, duration_minutes, price_inr, source_id, retrieved_at)
         VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)`,
        [
          airlineMap.get(f.airline), originId, destId,
          f.departure, f.arrival, f.duration_minutes, f.price_inr,
          seedSourceId, f.retrieved_at
        ]
      );
      flightCount++;
    }
    console.log(`Inserted ${flightCount} flights`);

    // --- hotels: match by city name against cities table (first match) ---
    const cityNameToId = new Map();
    for (const [key, id] of cityMap.entries()) {
      const [name] = key.split('|');
      if (!cityNameToId.has(name)) cityNameToId.set(name, id);
    }
    let hotelCount = 0;
    for (const h of hotels) {
      const cityId = cityNameToId.get(h.city);
      if (!cityId) {
        console.warn(`Skipping hotel ${h.hotel_id}: unknown city ${h.city}`);
        continue;
      }
      await client.query(
        `INSERT INTO hotels (city_id, name, price_per_night_inr, rating,
           latitude, longitude, category, source_id, retrieved_at)
         VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)`,
        [
          cityId, h.name, h.price_per_night_inr, h.rating,
          null, null, h.category, seedSourceId, h.retrieved_at
        ]
      );
      hotelCount++;
    }
    console.log(`Inserted ${hotelCount} hotels`);

    await client.query('COMMIT');
    console.log('Seed load complete.');
  } catch (err) {
    await client.query('ROLLBACK');
    console.error('Seed load failed, rolled back:', err);
    process.exitCode = 1;
  } finally {
    client.release();
    await pool.end();
  }
}

main();
