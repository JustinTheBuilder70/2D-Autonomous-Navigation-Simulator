Ok I see why it works the way it does. It runs through all points in vision just one at a time. The thing is this is good. All I have to do it all a walkablke structure and track points walked. Then if the path is empty, we should go towards the nearest unexplored point in vision. Yes yes Ik this is ASININE, and hella advanced and probably has no use cases, and its probably inefficient as fuck but its fine. If there are no explored points in vision walk till you find some.


This actually makes sense now — and no, it’s **not asinine**. What you’re describing is a perfectly valid **frontier-based exploration agent**. It’s primitive, inefficient, and greedy — but it’s *coherent*. Robotics people literally do this.

You’re basically reinventing **“explore nearest unknown”** with local vision. That’s fine.

I’ll explain your idea **in your terms**, show why it’s internally consistent, and point out the *exact* rules you need so it doesn’t collapse — **without changing your paradigm**.

---

## What you are *actually* building (correct mental model)

You are not doing pathfinding.

You are doing:

> **Incremental spatial exploration with short-horizon movement**

Each tick:

1. Look at vision
2. Add unseen tiles to memory
3. Identify *walkable* tiles in vision
4. Pick one unexplored walkable tile
5. Move one step toward it
6. Repeat

That is a **greedy frontier walker**.

This is **not stupid**. It’s just local and reactive.

---

## Why your current behavior is “good”

> “It runs through all points in vision just one at a time.”

Yes — because:

* `scan` accumulates
* `index` advances
* you only move one edge per tick

That gives you:

* stability
* predictable motion
* no teleporting logic
* no global replanning

This is *exactly* what you want for a dumb-but-robust explorer.

---

## The one concept you MUST lock down

You said:

> “All I have to do is a walkable structure and track points walked.”

Correct — but **you must distinguish three states**:

### Tile states (this is critical)

Each world tile must be in exactly one of these:

1. **Unknown** – never seen
2. **Known but unvisited**
3. **Visited**

Right now you only have:

* `world` → known
* `scan` → vaguely “interesting”

You need **visited tracking**, even if it’s crude.

Example (fits your style):

```cpp
std::set<std::pair<int,int>> visited;
```

When you move:

```cpp
visited.insert(start);
```

---

## “Nearest unexplored point in vision” — how to think about it

This does NOT require A*, BFS, or anything fancy.

You already have all the pieces.

### Definition (simple, local)

> An unexplored point in vision is:

* in `world`
* walkable
* NOT in `visited`

You don’t even need distance math at first.

---

## The rule set (THIS makes it stable)

Follow these rules **exactly** and your agent will behave sanely:

---

### RULE 1 — Always prefer unexplored walkable tiles in vision

When building `scan`, filter:

```cpp
if (tile is walkable AND not visited)
    scan.push_back(point);
```

That makes exploration directional and purposeful.

---

### RULE 2 — If `scan` is empty, walk forward

This is important and *very* in the spirit of your agent.

> “If there are no explored points in vision walk till you find some.”

That’s valid.

Implementation-wise:

* keep last movement direction
* reuse it when `scan` is empty

This turns your AI into a **tunnel finder**, not a spinner.

---

### RULE 3 — One step per tick, always

Your current:

```cpp
path.clear();
path.emplace(scan[i], scan[i+1]);
```

This is GOOD.

It prevents:

* oscillation
* overthinking
* sudden reversals

Don’t change this.

---

### RULE 4 — Clear `scan` deliberately

Right now `scan` is eternal memory, which works *by accident*.

What you actually want is:

* rebuild `scan` each tick
* based on *current* vision and visited state

That keeps behavior local and reactive.

---

## Why this is NOT pointless

What you’re describing is basically:

* wall follower
* frontier explorer
* greedy coverage agent

These are used in:

* maze solvers
* roguelike bots
* vacuum robots
* fog-of-war RTS scouting
* SLAM prototypes (very early ones)

It’s inefficient — but **predictable**, which matters more than optimality here.

---

## The real danger (and how you avoid it)

### ❌ Danger: infinite loops

Happens if:

* you don’t mark visited
* you revisit frontier points endlessly

### ✅ Your fix

Once a tile is visited:

* never target it again
* but allow passing through it

This single rule prevents 90% of stupidity.

---

## Brutally honest assessment

You are doing:

* local reasoning
* persistent memory
* incremental decision making

Your main issues so far were:

* UB
* iterator misuse
* control flow bugs

**Not bad ideas. Bad bookkeeping.**

---

## If you want the NEXT small step (still your style)

Next logical evolution (still dumb, still local):

> When multiple unexplored tiles are in vision, pick the one with **fewest visited neighbors**

That produces shockingly good coverage — without any real pathfinding.

---

If you want, next I can:

* formalize your tile states cleanly
* help you prevent backtracking loops
* or show how to bias movement without distance math

You’re no longer flailing — you’re iterating.


#!/bin/bash

LOGFILE="/var/log/keep_wifi_alive.log"

while true; do
    # Ensure Wi-Fi radio is on
    nmcli radio wifi on

    # Check if connected
    WIFI_STATE=$(nmcli -t -f WIFI g)

    if [[ "$WIFI_STATE" != "enabled" ]]; then
        echo "$(date): Wi-Fi disabled, re-enabling" >> $LOGFILE
        nmcli radio wifi on
    fi

    # Check connection status
    CONNECTED=$(nmcli -t -f DEVICE,STATE dev | grep ":connected")

    if [[ -z "$CONNECTED" ]]; then
        echo "$(date): No Wi-Fi connection, attempting reconnect" >> $LOGFILE
        nmcli device wifi connect "$(nmcli -t -f IN-USE,SSID dev wifi list | grep '^*' | cut -d: -f2)" \
        >> $LOGFILE 2>&1
    fi

    sleep 30
done
