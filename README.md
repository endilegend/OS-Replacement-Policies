# Page-Replacement Simulator – Programming Assignment 4

A small C program that simulates and compares four classic page-replacement algorithms—Optimal (OPT), First-In-First-Out (FIFO), Least-Recently-Used (LRU), and CLOCK—on a randomly generated stream of 2,000 page references. After running, it reports each algorithm’s page-fault rate and lets you inspect snapshot views of every algorithm’s frame contents at any reference index.

---

## Table of Contents
1. [Key Features](#key-features)
2. [Algorithms Implemented](#algorithms-implemented)
3. [Project Structure](#project-structure)
4. [Building](#building)
5. [Running](#running)
6. [Sample Session](#sample-session)
7. [Configuration Tips](#configuration-tips)
8. [Performance Notes](#performance-notes)
9. [To-do Ideas](#to-do-ideas)
10. [Author & License](#author--license)

---

## Key Features
- Generates a random reference string of length 2,000 from the range 0–512.
- Fully in-memory, no external input files required.
- Computes page-fault counts & hit rates for OPT, FIFO, LRU, and CLOCK.
- Interactive snapshot mode: enter an index to inspect the five references before and after that position, along with each algorithm’s frame contents.
- Simple, self-contained ANSI C (C11); builds on Linux, macOS, WSL, or any POSIX-like system.

---

## Algorithms Implemented

| Abbrev. | Strategy               | Replacement Criterion                                               |
|--------:|------------------------|----------------------------------------------------------------------|
| OPT     | Optimal                | Evict the page whose next use is farthest in the future. Requires full knowledge of the reference string. |
| FIFO    | First-in-first-out     | Evict the oldest loaded page (circular queue).                       |
| LRU     | Least-recently-used    | Evict the page that has not been referenced for the longest time.   |
| CLOCK   | Second-chance / CLOCK  | Pages sit on a circular list with a reference bit; give one “second chance” before eviction, mimicking an approximation of LRU. |

Each algorithm’s inner loop writes a history matrix (`history[2000][frameSize]`) so the snapshot viewer can display the state of every frame at every time step.

---

## Project Structure
```bash
.
├── program4.c          # Source file (this repository’s main focus)
└── README.md           # You are here
```
> Only one source file is required; headers such as `<pthread.h>` are included for potential future extensions but are not used in the current build.

---

## Building
You just need GCC or any C11-capable compiler.
```bash
# Clone or download the repo, then:
gcc -std=c11 -Wall -Wextra -O2 program4.c -o pager -pthread
```
> `-pthread` is harmless overhead today, but it lets you experiment with multithreading extensions later.

---

## Running
```bash
./pager
Enter frames allowed: 4
```
The program:
- Builds the 2,000-entry reference string.
- Runs OPT → FIFO → LRU → CLOCK.
- Prints a summary table of page-fault percentages.
- Prompts for snapshot indices until you type 9999 to quit.

---

## Sample Session
```text
$ ./pager
Enter frames allowed: 4

Frame Size: 4
Algorithm    PF Rate (%)
--------------------------
OPT         11.30%
FIFO        18.15%
LRU         14.60%
CLOCK       15.10%
--------------------------
Which frame snapshot to be displayed (Enter 9999 to end program): 425
Page Stream | OPT      FIFO     LRU      CLOCK
------------------------------------------------
420         |  12       34       12       12
421         |  12       34       12       12
422         |  40       34       12       12
423         |  40       34       12       12
424         |  40       34       12       12
425         |  81       81       81       81
426         |  81       81       81       81
427         |  81       81       81       81
428         |  81       81       81       81
429         |  81       81       81       81
430         |  81       81       81       81
Which frame snapshot to be displayed (Enter 9999 to end program): 9999
```
> Values above are illustrative; actual numbers vary each run.

---

## Configuration Tips
| Aspect                   | Where to Change                                 | Note                                                   |
|--------------------------|--------------------------------------------------|--------------------------------------------------------|
| Reference-string length  | `#define REF_LEN 2000` (create and update loops)| Must match the history matrix sizes.                  |
| Page-value range         | `randomInt(0, 512);`                             | Adjust upper bound as needed.                         |
| Random seed              | `srand(time(NULL));` in `main()` before filling | Ensures different strings across runs.                |
| Snapshot window size     | Loop bounds in snapshot viewer                  | Increase for a wider context (snapshot - 5 to +5).    |

---

## Performance Notes
| Algorithm | Time Complexity                | Space Complexity                  |
|-----------|--------------------------------|-----------------------------------|
| OPT       | O(N × F)                     | O(F × N) for history matrix       |
| FIFO      | O(N × F)                     | O(F × N)                        |
| LRU       | O(N × F)                     | O(F × N) + O(F) for `uses[]`     |
| CLOCK     | O(N × F) worst-case; amortized O(N) | O(F × N)              |
> (N = reference length, F = frame size)

For learning purposes, the history matrix keeps every state; a production simulator could stream outputs to avoid the ~F × N memory cost.

---

## To-do Ideas
- Parameterize reference-string length & value range via CLI flags.
- Accept trace files instead of random strings.
- Export results to CSV or JSON for plotting.
- Visualize frame movements in real time with `ncurses` or `SDL`.
- Add NRU, Second-Chance FIFO, or Aging algorithms for deeper comparison.
- Parallelize simulations with threads to benchmark multi-core gains.
- Add automated unit tests (e.g., known traces with deterministic output).

---

## Author & License
**Endi Troqe** – sophomore computer-science student, basketball lover, and systems tinkerer.

Released under the MIT License.

Feedback, issues, and pull requests are welcome—feel free to open a discussion in the repo!

Enjoy experimenting with page-replacement strategies 🚀

