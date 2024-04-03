
# Ocean Wave Simulation

This project simulates an ocean surface using OpenGL with GLUT. It generates various types of waves based on predefined parameters and renders them in a 3D environment.The project uses sine waves to simulate the ocean surface

## Features

- Simulates different types of waves:
  - Stormy waves
  - Large, rolling waves
  - Choppy, wind-generated waves
  - Medium waves with varying direction
- Allows navigation within the 3D environment:
  - Arrow keys for rotation
  - WASD keys for movement
  - Plus and minus keys for zooming
- Real-time rendering using OpenGL

## Files

- `main.cpp`: Contains the main program code.
- `wave.h`: Header file defining classes for waves and ocean simulation.
- `spectrum.txt`: Text file containing parameters for generating wave spectra.

## Requirements

- C++ compiler
- OpenGL
- GLUT
## Usage

To compile and run the ocean wave simulation, follow these steps:

1. Compile the `main.cpp` file with the following command:

    ```bash
    g++ main.cpp -o ocean_simulation -lfftw3 -lglut -lGL -lGLU
    ```

   This command compiles the source code and generates an executable named `ocean_simulation`.

2. Run the compiled executable:

    ```bash
    ./ocean_simulation
    ```

   This command executes the compiled program, launching the ocean wave simulation.

3. Use the following controls to navigate within the simulation:

   - Arrow keys: Rotate the view.
   - WASD keys: Move the camera.
   - Plus (+) and minus (-) keys: Zoom in and out.

## Future Work
- Add fft and gpu support
- use gestner and try different wave generating algorithms

Enjoy exploring the simulated ocean environment!


