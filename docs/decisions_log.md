# Decision Log

| Date | Area | Decision | Rationale |
|---|---|---|---|
| Aug 15 2026 | Frontend | React + Vite | Fast setup, standard tooling |
| Aug 15 2026 | Styling | Tailwind CSS | User preference |
| Aug 15 2026 | C++ integration | Subprocess (stdin/stdout) | Simplest MVP path; upgrade only if a real need emerges |
| Aug 15 2026 | Maps | Leaflet | Mapbox only justified if it materially improves the product |
| Aug 15 2026 | Geo scope | India-first + a few intl hubs (Dubai, Singapore, Bangkok, Paris, Amsterdam) | Small enough to hand-verify algorithm correctness; supports the multi-city TSP example |
| Aug 15 2026 | MVP scope | Flights + Hotels only, ground transport deferred | Avoid a second graph model before the core one is proven |
| Aug 15 2026 | Travel data provider | Amadeus self-service decommissioned July 17 2026 (confirmed via search). Re-evaluate Duffel or similar when real-API work is actually scheduled | Deferred — real-API integration is a later-phase concern |
| Aug 15 2026 | User accounts | Yes, later (post-MVP) | Not needed for algorithmic core |
| Aug 15 2026 | Interview signal priority | DSA/algorithms | Drives time allocation toward algorithm correctness + benchmarking over UI polish |
| Aug 15 2026 | Data model | Raw attributes only, no precomputed scores | Score is objective-dependent, computed at query time |
| **Aug 16 2026** | **Database — SUPERSEDES earlier assumption** | **PostgreSQL, not MongoDB. Final.** | v2 spec is explicit: MahaPath already demonstrates MongoDB; this project should demonstrate relational modeling, normalization, PK/FK reasoning, indexing, and EXPLAIN/query-tuning depth — real interview-relevant DBMS signal that a document store doesn't give. Does **not** affect the C++ engine (still takes plain JSON via subprocess) or the seed data files already built (they remain valid — become the source data Node loads into Postgres tables, not a replacement for the DB layer) |
| Aug 16 2026 | TSP framing — technical honesty correction | Teach/benchmark classic bitmask-DP TSP as a DSA exercise (dp[mask][i], brute-force oracle, greedy comparison). Do **not** claim the real flight-routing problem is "solved via TSP" — real multi-city routing needs a time-aware formulation where transitions depend on arrival time, available departures, and connection buffers, not fixed pairwise costs | v2 spec explicitly separates "learning version" (plain TSP, great DSA practice) from "production direction" (time-aware). Resume/interview claims must reflect this distinction, not blur it |
| Aug 16 2026 | API surface | Added `GET /api/benchmarks/...` as explicit dev/admin-only endpoint | Formalizes what was implicit before |
| Aug 16 2026 | **Timeline — under review** | Compressed ~16-day full-build plan (Aug 15→31) vs. v2 spec's own extended Sept–Nov roadmap. These conflict. **Needs explicit confirmation from the student before phasing is finalized.** | See open question in current conversation |

## Tiering (unchanged in principle, DB swap doesn't change what's Tier 0/1/2)

**Tier 0 (must ship, resume-claimable):** data model + seed data, Dijkstra w/ path
reconstruction, A* w/ heuristic + benchmark vs Dijkstra, bitmask-DP TSP (framed
honestly as a DSA exercise, not the production routing claim) + greedy comparison,
joint flight+hotel scoring, PostgreSQL schema + Node/Express API over C++ subprocess,
minimal React UI, benchmark harness + results table, README/architecture docs.

**Tier 1 (add if pace holds):** time-dependent routing/connection feasibility, Leaflet
map, single structured-output AI constraint-extraction call, Pareto frontier view.

**Tier 2 (explicitly deferred, not claimed on resume):** real travel API integration,
Redis caching, ground transport modeling, preference learning, user accounts.
