# Vector Similarity Search and Clustering

This repository contains an implementation for vector-based data searching and clustering. It includes:
- Locality Sensitive Hashing (LSH)
- Hypercube
- Clustering (k-medians with various methods)

## Features
- **Fast Approximate Nearest Neighbor Search**: Using LSH and Hypercube methods.
- **Configurable Clustering**: Supports multiple clustering methods.
- **Flexible Command-Line Interfaces**: Easy to use with various options.

## Compilation
To compile the project, run the following commands from the root directory:
```bash
make clean
make
```

## Usage
### Locality Sensitive Hashing (LSH)
Run the LSH executable with the following options:
```
./lsh -d <input file> -q <query file> -k <int> -L <int> -o <output file> -N <number of nearest> -R <radius>
```
- `-d`: Path to the input file containing the dataset.
- `-q`: Path to the query file.
- `-k`: Number of hash functions.
- `-L`: Number of hash tables.
- `-o`: Path to the output file.
- `-N`: Number of nearest neighbors to find.
- `-R`: Radius for range search.

### Hypercube
Run the Hypercube executable with the following options:
```
./cube -d <input file> -q <query file> -k <int> -M <int> -probes <int> -o <output file> -N <number of nearest> -R <radius>
```
- `-d`: Path to the input file containing the dataset.
- `-q`: Path to the query file.
- `-k`: Number of dimensions.
- `-M`: Maximum number of candidate points to check.
- `-probes`: Number of probes.
- `-o`: Path to the output file.
- `-N`: Number of nearest neighbors to find.
- `-R`: Radius for range search.

### Clustering
Run the Clustering executable with the following options:
```
./cluster -i <input file> -c <configuration file> -o <output file> -complete -m <Classic|LSH|Hypercube>
```
- `-i`: Path to the input file containing the dataset.
- `-c`: Path to the configuration file.
- `-o`: Path to the output file.
- `-complete`: (Optional) Output complete clustering information.
- `-m`: Method to use for clustering (`Classic`, `LSH`, or `Hypercube`).

## File Structure
- `hashtable/`: Hash tables for LSH.
- `hypercube/`: Implementations for the Hypercube approach.
- `images/`: Data structures for image handling.
- `kmedians/`: Clustering logic (k-medians).
- `utils/`: Helper functions and timing utilities.
- `main_lsh.c`, `main_cube.c`, `main_cluster.c`: Entry points for each executable.
- `cluster.conf`: Example configuration file.
- `Makefile`: Build instructions.

## License
This project is available under the MIT License.

## Contributors
- [Efstathios Siatras](https://github.com/efsiatras)
- [Vassilis Panagakis](https://github.com/vm-panag)
