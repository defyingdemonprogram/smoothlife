# SmoothLife
**SmoothLife** is a continuous generalization of Conway’s **Game of Life**, developed by **Stephan Rafler** in 2011. Unlike the classic Game of Life which operates on a discrete grid with binary (alive/dead) cell states and fixed neighbor rules, SmoothLife extends this concept into the continuous domain:

* **Cell states** are represented by continuous values between 0 and 1 (instead of binary).
* **Neighborhood influence** is calculated using circular regions and smooth kernel functions, allowing for fractional contributions.
* The system evolves using smooth, differentiable functions—resulting in more fluid and lifelike animations.

This continuous model enables rich, organic patterns such as **gliders**, **breathers**, and **oscillators**, and can even exhibit properties reminiscent of biological morphogenesis or fluid dynamics.

## Quick Start

#### Build

```bash
./build.sh
```

#### Run (Terminal Rendering)

```bash
./smoothlife_term
```

> **Note:** If the program exits immediately, try running it again.

## References

* [Generalization of Conway's "Game of Life" to a continuous domain - SmoothLife (Paper)](https://arxiv.org/abs/1111.1567)
