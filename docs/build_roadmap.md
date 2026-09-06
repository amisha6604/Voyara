# Build Roadmap — Aug 15 to Sept 15, 2026 (31 days)

Confirmed: hard stop Sept 15, then full switch to placement prep. Target: resume-
and-interview-defensible for both top-tier and mid-tier SDE screens — depth over
breadth, nothing claimed that isn't built and understood.

Each phase ends with a **resume-claim checkpoint**: exactly what you can truthfully
write on the resume if you stopped right there. This never goes backward.

---

## Phase 1 — Foundations (Aug 15–21, ~1 week)

**Goal:** environment working, graph engine loading real data, Dijkstra correct and tested, DB layer stood up.

- [ ] WSL2 + g++ + Node + git environment (in progress)
- [ ] Read graph theory + Dijkstra notes; implement `graph.cpp` (loading) and `dijkstra.cpp`
- [ ] Pass all 8 test cases in `docs/algorithm_test_cases.md`
- [ ] PostgreSQL installed locally; schema created from v2 spec §6 (cities, airports,
      airlines, flights, hotels, searches, itineraries, itinerary_legs,
      itinerary_hotels, data_sources, algorithm_runs)
- [ ] Seed JSON data loaded into Postgres tables (simple load script)
- [ ] Node/Express skeleton: one endpoint that shells out to the C++ engine via
      stdin/stdout and returns a Dijkstra result as JSON

**Resume checkpoint:** *"Built a graph-based flight routing engine in C++
(Dijkstra, adjacency-list representation) with a PostgreSQL-backed data layer and
a Node.js/Express API."*

---

## Phase 2 — Algorithmic depth I: A* + time-dependence (Aug 22–28, ~1 week)

**Goal:** A* implemented and honestly benchmarked; routing becomes time-aware, not just cost-aware.

- [ ] Teach heuristic admissibility (per v2 spec §9 correction — justify the
      heuristic against the actual objective, not just assume great-circle distance works)
- [ ] Implement A*, test against the same 8 cases as Dijkstra
- [ ] Run both on `data/benchmark/` (500 nodes/3000 edges), record nodes-explored +
      runtime into `algorithm_runs` table
- [ ] Implement connection-feasibility logic (min buffer, day-rollover, timezone
      handling) using the tight-connection trap already seeded in `flights.json`
- [ ] Joint flight+hotel scoring layer (raw attributes → normalized → weighted
      score per spec §4)

**Resume checkpoint:** *"Implemented and benchmarked Dijkstra and A* on a
time-aware weighted flight graph; built a joint flight+hotel optimization layer
combining multiple normalized objectives."*

---

## Phase 3 — Algorithmic depth II: multi-city + UI (Aug 29–Sept 4, ~1 week)

**Goal:** bitmask DP done and honestly framed; product becomes visible/usable.

- [ ] Teach bitmask DP for TSP (dp[mask][i], brute-force oracle for tiny n)
- [ ] Implement + test against brute force on 4-6 city instances
- [ ] Implement greedy nearest-neighbour comparison; measure solution gap
- [ ] **Explicitly document (in code comments + docs) that this is a DSA exercise,
      not the production multi-city solution** — ties to the v2 spec correction
- [ ] React + Vite + Tailwind: search form, priority selector, itinerary comparison
      cards, score breakdown display
- [ ] Leaflet map showing route + layover points

**Resume checkpoint:** *"Developed multi-city itinerary optimization using
bitmask DP, validated against a brute-force oracle and benchmarked against a
greedy heuristic; built an interactive React frontend for itinerary comparison
and route visualization."*

---

## Phase 4 — Differentiating features + AI layer (Sept 5–10, ~6 days)

**Goal:** the features that make this stand out, not just check boxes.

- [ ] Pareto frontier view (non-dominated itineraries, not forced into one score)
- [ ] Exact-vs-heuristic comparison surfaced in the UI (turns the benchmark into
      a visible feature)
- [ ] Constraint-violation transparency ("why wasn't this shown" for filtered
      candidates)
- [ ] AI layer, scoped tightly: ONE structured-output LLM call that extracts
      preferences from natural language into a validated JSON schema, feeding the
      deterministic optimizer. No tool-calling loop, no chat memory, no
      explanation-generation yet unless time allows.
- [ ] Deploy (frontend + backend both live somewhere reachable by a link)

**Resume checkpoint:** *"Integrated natural-language preference extraction
(LLM + validated structured output) with the deterministic optimizer; exposed
Pareto-optimal tradeoffs and exact-vs-heuristic algorithm comparisons in the UI."*

---

## Phase 5 — Documentation + interview weaponization (Sept 11–15, ~5 days)

**Goal:** everything defensible, nothing hand-wavy.

- [ ] README + demo instructions
- [ ] Architecture diagram (React → Node/Express → C++ engine, PostgreSQL)
- [ ] PostgreSQL ER diagram
- [ ] Complexity table for every algorithm actually implemented
- [ ] Final benchmark table (real numbers only, from `algorithm_runs`)
- [ ] Finalize resume bullets — cross-check every claim against actual code
- [ ] Expand interview question bank (graphs/A*/DP sections already exist —
      add PostgreSQL, API design, AI sections)
- [ ] Self-run mock interview: walk through the project out loud, no notes,
      time yourself on the 30-second / 1-minute / 5-minute versions (spec §29)

**Resume checkpoint:** the five bullets in v2 spec §25, each one true.

---

## What's explicitly cut, and stays cut

Real travel API integration, Redis, ground transport, user accounts, preference
learning. These stay in `docs/decisions_log.md` as documented future roadmap —
visible to an interviewer as "I know what's next and why I didn't build it yet,"
which is itself a good answer to "what would you do with more time."

## If a phase runs long

Cut from the back (Phase 4 first, then late Phase 3 items), never from the front.
Phases 1-2 are the actual algorithmic core the whole project's credibility rests
on — they are not negotiable under time pressure.
