// server/src/index.js
const express = require('express');
const { callEngine } = require('./engineBridge');

const app = express();
app.use(express.json());

app.get('/api/health', (req, res) => {
  res.json({ status: 'ok' });
});

// First real endpoint: a direct pass-through to the engine's shortest-path
// command, so you can verify the whole chain (React later -> Express -> C++
// -> Express -> React) works before building the full optimize/itinerary
// pipeline on top of it.
//
// Once engine/src/main.cpp reads stdin JSON and calls dijkstra() (your TODO),
// this endpoint should work end-to-end:
//   curl -X POST localhost:3001/api/route \
//     -H "Content-Type: application/json" \
//     -d '{"origin":"VNS","destination":"DEL","weightType":"PRICE"}'
app.post('/api/route', async (req, res) => {
  const { origin, destination, weightType } = req.body;

  if (!origin || !destination) {
    return res.status(400).json({ error: 'origin and destination are required' });
  }

  try {
    const result = await callEngine({
      command: 'shortest_path',
      origin,
      destination,
      weightType: weightType || 'PRICE'
    });
    res.json(result);
  } catch (err) {
    // Never fabricate a result if the engine fails — spec Section 18/26 rule.
    res.status(502).json({ error: 'Engine call failed', detail: err.message });
  }
});

const PORT = process.env.PORT || 3001;
app.listen(PORT, () => {
  console.log(`Travel Optimizer API listening on port ${PORT}`);
});
