const express = require('express');
const { callEngine } = require('./engineBridge');
const { query } = require('./db');

const app = express();

app.use(express.json());


// --------------------------------------------------
// Health check
// --------------------------------------------------

app.get('/api/health', (req, res) => {
    res.json({ status: 'ok' });
});


// --------------------------------------------------
// Get flights directly from PostgreSQL
// --------------------------------------------------

app.get('/api/flights', async (req, res) => {

    const { origin, destination } = req.query;

    if (!origin || !destination) {
        return res.status(400).json({
            error: 'origin and destination are required'
        });
    }

    try {

        const result = await query(
            `
            SELECT
                f.flight_id,
                ao.iata AS origin,
                ad.iata AS destination,
                al.name AS airline,
                f.departure_at,
                f.arrival_at,
                f.duration_minutes,
                f.price_inr,
                f.status
            FROM flights f
            JOIN airports ao
                ON f.origin_airport = ao.airport_id
            JOIN airports ad
                ON f.destination_airport = ad.airport_id
            JOIN airlines al
                ON f.airline_id = al.airline_id
            WHERE ao.iata = $1
              AND ad.iata = $2
              AND f.status = 'active'
            ORDER BY f.departure_at;
            `,
            [
                origin.toUpperCase(),
                destination.toUpperCase()
            ]
        );

        res.json({
            count: result.rows.length,
            flights: result.rows
        });

    } catch (err) {

        console.error('Database error:', err);

        res.status(500).json({
            error: 'Failed to fetch flights',
            detail: err.message
        });
    }
});


// --------------------------------------------------
// Route optimization
// PostgreSQL → Node → C++
// --------------------------------------------------

app.post('/api/route', async (req, res) => {

    const {
        origin,
        destination,
        weightType
    } = req.body;

    if (!origin || !destination) {
        return res.status(400).json({
            error: 'origin and destination are required'
        });
    }

    try {

        // 1. Fetch flight data from PostgreSQL
        const flightResult = await query(
            `
            SELECT
                f.flight_id,
                ao.iata AS origin,
                ad.iata AS destination,
                al.name AS airline,
                TO_CHAR(f.departure_at, 'YYYY-MM-DD"T"HH24:MI:SS') AS departure,
TO_CHAR(f.arrival_at, 'YYYY-MM-DD"T"HH24:MI:SS') AS arrival,
                f.duration_minutes,
                f.price_inr,
                f.status
            FROM flights f
JOIN airports ao
    ON f.origin_airport = ao.airport_id
JOIN airports ad
    ON f.destination_airport = ad.airport_id
JOIN airlines al
    ON f.airline_id = al.airline_id
WHERE f.status = 'active'
            ORDER BY f.departure_at;
            `
        );


        // 2. Convert PostgreSQL rows into engine input
        const flights = flightResult.rows.map(flight => ({
            flight_id: String(flight.flight_id),
            origin: flight.origin,
            destination: flight.destination,
            airline: flight.airline,
            departure: flight.departure,
            arrival: flight.arrival,
            price_inr: flight.price_inr,
            duration_minutes: flight.duration_minutes
        }));


        // 3. Send the database data to C++
        const result = await callEngine({
            command: 'shortest_path',
            origin: origin.toUpperCase(),
            destination: destination.toUpperCase(),
            weightType: weightType || 'PRICE',
            flights
        });


        // 4. Return C++ result to client
        res.json(result);

    } catch (err) {

        console.error('Route optimization error:', err);

        res.status(502).json({
            error: 'Route optimization failed',
            detail: err.message
        });
    }
});


const PORT = process.env.PORT || 3001;

app.listen(PORT, () => {
    console.log(`Travel Optimizer API listening on port ${PORT}`);
});