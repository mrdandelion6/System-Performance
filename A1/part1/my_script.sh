#!/bin/bash

pip install matplotlib
pip install pandas

echo "Running C executable..."
./part1

echo "Running Python script..."
python3 gen/generate_graphs.py

echo "Done!"