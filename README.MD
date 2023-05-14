# Building the Project with CMake
## Project Description

This is a C project that includes multiple files and dependencies. This project implements the input, storage and output of graphs in various formats.

## Dependencies

To build the project, you need to install the following dependencies:
* CMake
* C Compiler (e.g. GCC)
* GraphViz (For installation use the link https://graphviz.org/download/ , download the installer and install the program on your PC. Add it to the system patch.)

## Cloning the Repository

Before starting the build process, you need to clone the repository to your local computer

``` git clone https://github.com/andreu5547/FLaTOA_hw2.git ```

## Building the Project with CMake
1. Navigate to the root directory of the project:
```cd FLaTOA_hw2```
2. Create a "build" directory:
```mkdir build```
3. Navigate to the "build" directory:
```cd build```
4. Generate the project files with CMake:
```cmake ..```
5. Build the project:
```cmake --build .```
6. Profit

## Run
* Usage: ```Usage: FLaTOA_hw2 <input filename> <directed> <weighted> <del_repeat>```
* Arguments:
    1. \<input filename>: Name of the file with the graph // string
    2. \<directed>: Directed or undirected graph // true or false (1, 0)
    3. \<weighted>: Weighted or unweighted graph // true or false (1, 0)
    4. \<del_repeat>: Delete duplicate edges in a graph // true or false (1, 0)
* Example: ```Example: FLaTOA_hw2 graph.txt true false true```