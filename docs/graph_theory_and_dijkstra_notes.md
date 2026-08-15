# Graphs + Dijkstra — Read This Before You Code

Goal: by the end of this, you should be able to explain Dijkstra to an interviewer
without looking at your own code. The implementation tomorrow is just translating
this understanding into C++ — if the understanding is solid, the code is mechanical.

## 1. Why a graph at all?

Airports are natural "things" and flights are natural "connections between things."
That's literally the definition of a graph: **nodes** (airports) + **edges** (flights).
Once you frame it this way, "find the best route from Varanasi to Paris" becomes
"find the best path between two nodes in a graph" — a solved, well-studied problem,
instead of something you'd have to invent from scratch.

## 2. Weighted, directed graph

- **Directed**: a VNS→DEL flight existing doesn't mean a DEL→VNS flight exists at
  the same time/price. Edges have a direction. (Your seed data already has separate
  return legs for this reason.)
- **Weighted**: each edge (flight) has a cost — could be price, could be duration,
  could be a blended score. The "weight" is whatever you're optimizing for right now.
- **Adjacency list** is how you'll store this: for each airport, a list of
  "(flight to X, weight)" pairs. Not an adjacency *matrix* (a grid of every airport
  vs every airport) — with 15 airports a matrix would work fine, but it wastes
  memory on real-scale data (most airport pairs have no direct flight) and doesn't
  scale. Adjacency list is the standard real-world choice.

## 3. The core idea behind Dijkstra

You're at Varanasi (cost 0 to reach). Every other airport starts at "infinite cost
to reach" because you don't know a path yet.

Repeat:
1. Pick the **unvisited** airport with the **smallest known cost** so far.
2. Look at all its direct flights. For each one, check: is going through *this*
   airport cheaper than however we currently think we can reach the destination?
   If yes, update that destination's cost — this is called **relaxing an edge**.
3. Mark the current airport visited. Never reconsider it.
4. Repeat until every reachable airport is visited.

The reason step 1 (always pick the smallest known cost next) works is the
**greedy invariant**: once you pick the airport with the smallest tentative cost
among all unvisited airports, that cost is already final — nothing you haven't
visited yet could possibly offer a cheaper path to it, *because all edge weights
are non-negative*. If weights could be negative, a not-yet-visited airport could
still offer a sneaky cheaper path later, and the whole guarantee breaks. This is
exactly why Dijkstra requires non-negative weights and Bellman-Ford exists for
graphs that don't have that guarantee.

## 4. Why a priority queue?

Step 1 above — "pick the unvisited node with smallest known cost" — is the
expensive part if done naively (scanning every node every time = O(V) per step,
O(V²) total). A **min-priority queue** (min-heap) lets you get the smallest
element in O(log V) instead, which is why real Dijkstra implementations use one.
You'll push (cost, airport) pairs in, and always pop the smallest.

**Important gotcha**: a node can get pushed into the priority queue multiple
times (once per time its cost gets updated/relaxed). When you pop a node, check
if you've already finalized it — if so, skip it, don't reprocess. This is the
single most common bug in Dijkstra implementations.

## 5. Complexity

With adjacency list + binary heap priority queue:
**O((V + E) log V)** — V = airports, E = flights. You should be able to derive
this, not just recite it: each edge can trigger one push to the heap (E pushes,
each O(log V)), each vertex is popped once (V pops, each O(log V)).

## 6. Reconstructing the actual path (not just the cost)

Dijkstra as described above only tells you the *cost* to reach each airport. To
get the actual route (which flights, in order), you keep a `previous[]` array:
whenever you relax an edge and update a node's cost, also record "I reached this
node via this edge from this previous node." At the end, walk backwards from the
destination through `previous[]` until you hit the origin, then reverse it.

This matters a lot for this project specifically — the spec is explicit (§10.1)
that you need the *path*, not just a number, because the whole product is about
showing itineraries.

## 7. Edge cases to keep in your head (your seed data has all of these on purpose)

- **Disconnected node** (Port Blair/IXZ) — algorithm must terminate and report
  "no path found," not crash or loop forever.
- **Multiple edges between the same pair** (e.g. two VNS→DEL flights) — these are
  NOT duplicates, they're genuinely different options (different price/time). Your
  graph structure must allow parallel edges.
- **Same source and destination** — cost should be 0, trivial but worth testing
  explicitly so you're not surprised later.
- **Equal-cost paths** — two different routes with identical total cost. Dijkstra
  will find *a* shortest path, not necessarily deterministically "the same one"
  across implementations — worth knowing so you don't panic if your answer differs
  from an example by picking a different equally-valid route.

## 8. What Dijkstra does NOT do here (yet)

Plain Dijkstra assumes all edges are always "available" — it doesn't know that a
12:20 DEL→DXB flight is actually unreachable if you land at 12:00 and need 45 min
minimum connection. That's **time-dependent routing** (spec §11), a layer on top
of Dijkstra, not Dijkstra itself. Don't try to solve that tomorrow — get plain
Dijkstra correct first (shortest path by price or by duration, ignoring real-world
timing feasibility), then we add the time-dependent constraint as its own step.

## 9. What you should be able to say out loud before writing code

If you can explain these five things in your own words, you're ready to implement:
1. Why is this a graph problem at all?
2. Why adjacency list over adjacency matrix here?
3. Why does the greedy "pick smallest known cost" step work — what breaks it?
4. Why a priority queue, and what's the complexity?
5. How do you get the actual path out, not just the cost?
