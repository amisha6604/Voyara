# Interview Bank — Graphs & Dijkstra

Study these alongside building, not after. If you can answer all of these once
Dijkstra is working, that piece of the project is interview-ready.

## Conceptual
- Why represent airports/flights as a graph instead of some other structure?
- Why a weighted graph rather than unweighted?
- Why adjacency list rather than adjacency matrix, specifically for this dataset?
- Why does Dijkstra require non-negative edge weights? What would break with a
  negative weight, concretely — walk through an example.
- What is "relaxing an edge" — say it in one sentence.
- Why a priority queue instead of scanning for the minimum each time? What's the
  complexity difference?
- What is the time complexity of your implementation, and why — derive it, don't
  just state it (V and E in terms of pushes/pops).
- How do you reconstruct the actual path, not just the shortest cost?

## This-project-specific
- Your graph has multiple flights between the same city pair (different price,
  different time). How does your graph structure handle that without losing
  options?
- What happens if a queried airport doesn't exist in the graph at all vs. exists
  but has zero outgoing flights (Port Blair)? Are these handled the same way, and
  should they be?
- Same city pair, two different objectives (cheapest vs fastest) can return two
  different routes. Why does your engine support that without duplicating the
  graph?
- What's the actual difference between what plain Dijkstra computes and what a
  full itinerary the product shows (hint: connection feasibility, layovers)?

## Failure / edge-case probing (interviewers love these)
- What does your engine return if there's genuinely no path? Walk through what
  happens in your code, not just "it returns false."
- Two routes have identical total cost — does your implementation always return
  the same one? Does it matter that it might not?
- What if the same flight_id somehow appeared twice in the data (bad data)? Would
  your engine misbehave?

## Bridge to A* (once you get there)
- Why would you use A* instead of just always using Dijkstra?
- What is an admissible heuristic, and what heuristic makes sense for flight
  routing specifically (hint: great-circle distance, spec mentions this)?
- How do you prove/verify your heuristic is actually admissible for this graph?
- If your heuristic were NOT admissible, what would go wrong — would you get an
  error, or something more subtle?
