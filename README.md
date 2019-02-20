# Kiva4Reader

This repository contains the Kiva4Reader for VTK.

Reads a dataset in KIVA4 format (kiva4grid)

# Overview

vtkKiva4Reader creates an unstructured grid dataset. It reads ASCII files stored in KIVA4 format (i.e., kiva4grid), with boundary data at cells of the model. The OutputFace option controls the mesh types for outputting:
  =0: only volumes
  =1: volumes and faces except for interior faces
  =2: volumes and all faces

# Compiling from scratch requires CMake.

    git clone https://github.com/Kenichiro-Yoshimi/Kiva4Reader.git
    cd Kiva4Reader
    cmake .
    make

# Test

    ./kiva4ReaderTest kiva4grid 0

# Sample Visualization

![Outputs examples for different coptions](https://i.imgur.com/GTELx6p.png)


