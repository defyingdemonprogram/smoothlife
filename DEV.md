# SmoothLife

**SmoothLife** is a mathematical enhancement of Conway's Game of Life. While the original Game of Life uses a discrete grid and binary states (alive/dead), SmoothLife transitions these concepts into a **continuous domain**. This allows for fluid, organic patterns that more closely resemble biological processes or fluid dynamics.

## Use Cases
SmoothLife and similar continuous cellular automata have applications beyond mere visual curiosity:

1.  **Biological Morphogenesis**: Modeling how organisms develop their shape and patterns (e.g., skin patterns on animals).
2.  **Fluid Dynamics & Chemistry**: Simulating reaction-diffusion systems where chemicals interact and spread.
3.  **Procedural Content Generation**: Creating organic textures, maps, or terrain for games and simulations.
4.  **Mathematical Research**: Studying emergent behavior in continuous dynamical systems.
5.  **Art and Aesthetics**: Generating high-fidelity organic animations and visual effects.

## Logic and Algorithm Breakdown

The implementation in `smoothlife_term.c` follows these core principles:

### 1. Continuous State Space
Unlike the classic 0 or 1 state, every "cell" in the grid holds a floating-point value between `0.0` and `1.0`. 
- `0.0` represents "dead".
- `1.0` represents "fully alive".
- Values in between represent varying degrees of "aliveness".

### 2. Neighborhood Definition
Instead of checking exactly 8 neighbors, SmoothLife looks at circular regions around a point:
- **Inner Radius ($r_i$)**: Defines the "nucleus" or the cell's own state.
- **Outer Radius ($r_a$)**: Defines the "neighborhood" or the "environment" influence.

In our implementation:
- $r_a$ is defined as `ra = 11.0`.
- $r_i$ is calculated as $r_a / 3.0$.

For every point $(cx, cy)$, the algorithm calculates:
- $m$: The average state within the inner radius $r_i$.
- $n$: The average state within the ring between $r_i$ and $r_a$.

### 3. Transition Functions (Smoothing)
SmoothLife uses **Sigmoid functions** to avoid hard transitions. This ensures that a small change in population results in a small change in the next state, rather than a sudden jump.

- **`sigma(x, a, alpha)`**: A standard sigmoid function that transitions from 0 to 1 around value `a`.
- **`sigma_n(x, a, b)`**: A "bell-shaped" transition that is high between `a` and `b` and low elsewhere.
- **`sigma_m(x, y, m)`**: Interpolates between two values based on the inner state $m$.

### 4. Survival and Birth Rules
The core "rules" are defined by the function `s(n, m)`:
- It determines the "ideal" next state based on the current neighborhood density ($n$) and the current local state ($m$).
- Parameters like `b1, b2, d1, d2` define the "Birth" and "Death/Survival" thresholds.

### 5. State Integration
Instead of replacing the old state with the new one, the system calculates a **difference** (`grid_diff`):
$$ \Delta \text{grid} = 2 \cdot s(n, m) - 1 $$
This difference is then applied to the grid using a time step `dt`:
$$ \text{grid} = \text{grid} + dt \cdot \Delta \text{grid} $$
This is effectively a simple **Euler integration**, allowing the pattern to evolve smoothly over time.

---
*For more technical details, refer to Stephan Rafler's original paper: [Generalization of Conway's "Game of Life" to a continuous domain - SmoothLife](https://arxiv.org/abs/1111.1567)*
