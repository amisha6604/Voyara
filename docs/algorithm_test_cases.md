# Dijkstra Test Cases — Verify Against These Once Implemented

Run each of these manually (print results from main.cpp, or write a small test
loop) once `dijkstra()` compiles. Check the expected behavior column before
trusting any result.

| # | Query | Weight | Expected behavior | Why this matters |
|---|---|---|---|---|
| 1 | VNS → DEL | PRICE | Found, cost = 3900 (the cheaper of the two direct flights) | Basic correctness + picks the cheaper of two parallel edges |
| 2 | VNS → DEL | DURATION | Found, cost = 90 (both direct flights are 90 min, either is fine) | Same route, different objective — same graph, different answer. This is the core promise of the whole product (spec §4.3) |
| 3 | VNS → IXZ | PRICE | found = false | Disconnected node — must terminate cleanly, not crash/hang |
| 4 | VNS → VNS | PRICE | found = true, cost = 0, path = [VNS] | Trivial same-source-destination case |
| 5 | VNS → DXB | PRICE | Found via VNS→DEL→DXB (multi-hop, picks the DEL→DXB option that yields lowest total, likely the 19500 one over the 21000 one) | Verifies multi-hop path reconstruction, not just single edges |
| 6 | VNS → CDG | PRICE | Found via a 2-3 hop path through DEL/DXB | Longer chain, good stress test for path reconstruction |
| 7 | DEL → BOM | PRICE | Found, cost = 3800 (picks IndiGo over Vistara's 4100) | Confirms parallel-edge tie-breaking doesn't silently pick the wrong one |
| 8 | IXC → COK | PRICE | Found, but likely a longer multi-hop path since no direct flight exists | No direct edge exists — confirms the algorithm actually searches, doesn't just check direct edges |

## Benchmark checks (once you're comparing against A*)

Use `data/benchmark/airports_bench.json` + `flights_bench.json` (500 nodes, 3000
edges) for these — the small seed data is too fast to measure meaningfully.

- Run Dijkstra on 10+ random origin/destination pairs, record `nodesExplored` and
  `runtimeMs` for each.
- Once A* exists, run the *same* pairs through both and compare nodesExplored —
  A* should explore fewer nodes on average (that's the whole point of the
  heuristic), while both should return the *same* totalCost for a given pair
  (this is the actual correctness check for A* — if A* ever returns a worse cost
  than Dijkstra for the same query, your heuristic is not admissible and something
  is wrong).

## What "done" looks like for this step

You can run test cases 1-8 above, get the expected results, and explain out loud
*why* each one is expected — not just that the number matched.
