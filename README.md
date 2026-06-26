# Time-Travelling File System

## Table of Contents
- [Introduction](#introduction)
- [Core Commands](#core-commands)
- [How to Build and Run](#how-to-build-and-run)
- [Author](#author)

---

## Introduction

This project implements a **Time-Travelling File System** which manages files and their version histories in memory. It supports all functions as said in [Core Commands](#core_commands)

It is inspired by Git and the main goal is to be able to use Data Structures like Hash Maps, Heaps and Trees.

---

## Core Commands

Commands are read from standard input and executed sequentially:

- `CREATE <filename>`: Creates a new file with root version (ID 0), empty content, initial snapshot message.
- `READ <filename>`: Displays content of the active version.
- `INSERT <filename> <content>`: Appends content. If the active version is snapshotted, creates a new version; else modifies the existing version.
- `UPDATE <filename> <content>`: Replaces active version content. Versioning follows same logic as INSERT.
- `SNAPSHOT <filename> <message>`: Marks active version as immutable snapshot with a message.
- `ROLLBACK <filename> [versionID]`: Sets active version to the specified version or to its parent if no version ID given.
- `HISTORY <filename>`: Lists all snapshotted versions from active node to root with version ID, timestamp, and message.
- `EXIT`: Exits the program.
- `RECENT_FILES [num]`: Lists files in descending order of last modification time (limit `num`).
- `BIGGEST_TREES [num]`: Lists files with largest version trees by version count (limit `num`).

---

## How to Build and Run

| Platform | Instructions |
| -------- | ------------ |
| **Windows** | Use the provided `run.bat` batch script: <br>1. Open Command Prompt and navigate to the project directory. <br>2. Run `.\run.bat`. |
| **Linux** | Use the provided `run.sh` shell script: <br>1. Open a terminal and navigate to the project directory. <br>2. Make the script executable (if needed) with `chmod +x run.sh`. <br>3. Run `./run.sh`. |


## Author

Arnav Meena, 2024CS50218   
IIT Delhi