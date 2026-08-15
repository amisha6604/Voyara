# Decision Log

| Date | Area | Decision | Rationale |
|---|---|---|---|
| Aug 15 2026 | Frontend | React + Vite | Fast setup, standard tooling, matches spec's React recommendation |
| Aug 15 2026 | Styling | Tailwind CSS | User preference |
| Aug 15 2026 | C++ integration | Subprocess (stdin/stdout) | Simplest MVP path per spec §20.3; upgrade to HTTP service only if a real need emerges |
| Aug 15 2026 | Maps | Leaflet | Mapbox only justified if it materially improves the product (spec §22) — no time to learn a new API given deadline |
| Aug 15 2026 | Geo scope | India-first + a few intl hubs (Dubai, Singapore, Bangkok, Paris, Amsterdam) | Keeps seed data small enough to hand-verify algorithm correctness; intl hubs added specifically to support the multi-city TSP example from spec §12.1 |
| Aug 15 2026 | MVP scope | Flights + Hotels only, ground transport deferred | Spec §33 scope control — avoid building a second graph model before the core one is proven |
| Aug 15 2026 | Travel data provider | Amadeus self-service portal was decommissioned July 17, 2026 (confirmed via search). Re-evaluate Duffel or similar when real-API integration is actually scheduled. | Original spec assumption (Amadeus) is stale; deferred since real-API work is Tier 2 |
| Aug 15 2026 | User accounts | Yes, later (post-MVP) | Not needed for algorithmic core or resume claims |
| Aug 15 2026 | Interview signal priority | DSA/algorithms | Drives how implementation time is allocated — more time on Dijkstra/A*/bitmask DP correctness + benchmarking than on UI polish |
| Aug 15 2026 | **Timeline (major replan)** | Full "dream project" scope compressed into ~16 days (by Aug 31), because placement prep takes over after that — not the original Aug 31 MVP → Nov dream-project timeline the spec assumed | User must stop building and switch to interview/placement prep after Aug 31; whatever isn't built by then does not go on the resume |
| Aug 15 2026 | Data model | Raw attributes only, no precomputed scores | Score is objective-dependent and computed at query time (spec §9.3, §16.1) |

## Tiering under the compressed timeline

**Tier 0 (must ship, resume-claimable):** data model + seed data, Dijkstra w/ path
reconstruction, A* w/ heuristic + benchmark vs Dijkstra, bitmask DP TSP + greedy
comparison, joint flight+hotel scoring, Node/Express API over C++ subprocess, minimal
React UI, benchmark harness + results table, README/architecture docs.

**Tier 1 (add if pace holds):** time-dependent routing/connection feasibility, Leaflet
map, single structured-output AI constraint-extraction call, Pareto frontier view.

**Tier 2 (explicitly deferred, not claimed on resume):** real travel API integration,
Redis caching, ground transport modeling, preference learning, user accounts.

Rule: nothing in Tier 1/2 gets claimed on the resume unless it is actually built and
the student can defend it in an interview.
