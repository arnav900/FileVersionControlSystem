#!/bin/bash
g++ -std=c++11 main.cpp -o main

# Check if compilation was successful
if [ $? -eq 0 ]; then
  echo "Compilation successful. Running program..."
  ./main
else
  echo "Compilation failed."
fi