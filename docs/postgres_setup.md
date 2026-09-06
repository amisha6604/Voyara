# PostgreSQL Setup (WSL/Ubuntu)

This was missing from the original environment checklist — do this alongside
or after the g++/Node/git setup.

## 1. Install PostgreSQL

In your Ubuntu terminal:
```
sudo apt update
sudo apt install postgresql postgresql-contrib -y
```

## 2. Start it and create your database

```
sudo service postgresql start
sudo -u postgres psql
```

Inside the `psql` prompt that opens:
```sql
CREATE DATABASE travel_optimizer;
CREATE USER traveluser WITH PASSWORD 'devpassword';
GRANT ALL PRIVILEGES ON DATABASE travel_optimizer TO traveluser;
\q
```

(Use a real password if you want, `devpassword` is fine for local dev only —
never used anywhere but your own machine.)

## 3. Set environment variables so `pg` (Node) and `psql` can connect

Add to `~/.bashrc` (open with `nano ~/.bashrc`, add at the bottom, save with
Ctrl+O then Enter, exit with Ctrl+X), then run `source ~/.bashrc`:
```
export PGHOST=localhost
export PGUSER=traveluser
export PGPASSWORD=devpassword
export PGDATABASE=travel_optimizer
export PGPORT=5432
```

## 4. Apply the schema

From the project root:
```
psql -h localhost -U traveluser -d travel_optimizer -f server/db/schema.sql
```

## 5. Load seed data

```
cd server
npm install
node db/seed_loader.js
```

You should see output like:
```
Inserted 15 cities
Inserted 15 airports
Inserted 12 airlines
Inserted 36 flights
Inserted 41 hotels
Seed load complete.
```

## 6. Verify

```
psql -h localhost -U traveluser -d travel_optimizer -c "SELECT count(*) FROM flights;"
```
Should return 36 (or however many made it in — check the warnings from step 5
if it's lower, that means some airport/city name didn't match).

## Quick sanity queries worth running (and understanding, for interviews)

```sql
-- All direct flights from Varanasi
SELECT f.flight_id, a2.iata AS destination, f.price_inr, f.duration_minutes
FROM flights f
JOIN airports a1 ON f.origin_airport = a1.airport_id
JOIN airports a2 ON f.destination_airport = a2.airport_id
WHERE a1.iata = 'VNS';

-- Check that your indexes are actually being used
EXPLAIN ANALYZE
SELECT * FROM flights WHERE origin_airport = (SELECT airport_id FROM airports WHERE iata = 'VNS');
```

Run the `EXPLAIN ANALYZE` one and read the output — this is exactly the kind
of thing v2 spec's interview checklist expects you to be comfortable with
(§23: "EXPLAIN, transactions, pooling, scaling").
