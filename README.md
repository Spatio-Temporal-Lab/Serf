# *Serf*: Streaming Error-bounded Floating-Point Compression

*Serf* is a fast compression algorithm with high compression performance, which is designed specifically for streaming applications and time-series floating-point data.

## :nut_and_bolt:Prerequisite

- For Windows 10/11, please install `WSL` first and follow instructions for Linux environments.

  :bell:*Tips*

  If you are a C++ expert, you can convert this CMake project into a Visual Studio project using auto-converting tools.

- For MacOS, please install `Xcode Command Line Tools` and a package manager like `Homebrew`.

  Then, please install `git`, `cmake` and `clang` using your package manager.

  ```bash
  brew install git cmake clang
  ```

- For Linux (Ubuntu 20.04 and above; Fedora 42 and above), please make sure your package manager works.

  Then, please install `git`, `cmake` and `gcc` using your package manager.

  Example:

  ```bash
  sudo apt update && sudo apt upgrade
  sudo apt install -y git cmake build-essential
  ```

## :hammer:Building

1. Clone the repository:

   ```bash
   git clone git@github.com:Spatio-Temporal-Lab/Serf.git
   cd Serf
   ```

3. Generate CMake building system:

   ```bash
   cmake -S . -B ./build
   ```

3. Build *Serf*

   ```bash
   cmake --build ./build --target serf --config release -j{$CORE_NUMS}
   ```

   :bell:*Tips*

   Replace `{$CORE_NUMS}` with the number of your CPU cores or a number lower. This would bring you out from boring time of waiting.

   If you want to build *Serf* with command-line interface, please run:

   ```bash
   cmake --build ./build --target sfz --config release -j{$CORE_NUMS}
   ```

   If you want to build the benchmark program, please run:

   ```bash
   cmake --build ./build --target PerformanceProgram --config release -j{$CORE_NUMS}
   ```

   If you want to build all targets, please run:

   ```bash
   cmake --build ./build --config release -j{$CORE_NUMS}
   ```

## :floppy_disk:Usage

### Command-line interface

Once you have built *Serf* with command-line interface, you can run it in the console.

Example: (Assume your working directory is the root directory of the source code)

```bash
# Compression
./build/serf_cmd/sfz compress 0.01 ./input.txt ./output.sfz

# Decompression
./build/serf_cmd/sfz decompress 0.01 ./input.sfz ./output.txt

# Get help
./build/serf_cmd/sfz
```

:bell:*️Tips*

This is only a demo program, so it might not work under certain circumstances.

### As a library

The demo program mentioned above is a good example for you to learn how to link *Serf* to your program.

Check the `CMakeLists.txt` in the `serf_cmd`:

```cmake
cmake_minimum_required(VERSION 3.10)

project(sfz)

# Your program here
add_executable(sfz cmd_interface.cc)

# Add the source code directory of Serf to your including directories
target_include_directories(sfz PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Link Serf to your program
target_link_libraries(sfz PRIVATE serf)
```

## :file_folder:Project Structure

### Overview

```
.
├── CMakeLists.txt			# Global CMake building configs
├── CODE_OF_CONDUCT.md		# Code of Conduct
├── pywrapper				# Python interface of Serf
├── README.md				# README manuals
├── serf_cmd				# Command-line interface of Serf
├── src						# Source code of Serf
└── test					# Unit tests and benchmark programs
```

### Core of *Serf*

```
src
├── CMakeLists.txt		# Local CMaker building configs
├── compressor			# Compression algorithms
├── compressor_32		# 32-bit compression algorithms
├── decompressor		# Decompression algorithms
├── decompressor_32		# 32-bit decompression algorithms
└── utils				# utility files for supporting compression and decompression
```

#### Compressor

- **`net_serf_qt_compressor`**: Network-based compressor using the *Serf-Qt* algorithm.
- **`net_serf_xor_compressor`**: Network-based compressor using the *Serf-XOR* algorithm.
- **`serf_qt_compressor.`**: Standard *Serf-Qt* compression implementation.
- **`serf_xor_compressor`**: Standard *Serf-XOR* compression implementation.
- **`serf_xor_compressor_no_fast_search`**: *Serf-XOR* compressor without fast search optimizations.
- **`serf_xor_compressor_no_opt_appr`**: *Serf-XOR* compressor without optimized approximations.
- **`serf_xor_compressor_rel`**: *Serf-XOR* compressor in relative error-bound mode.

#### Compressor (32-bit)

- **`serf_qt_compressor_32`**: 32-bit implementation of the *Serf-Qt* compressor.
- **`serf_xor_compressor_32`**: 32-bit implementation of the *Serf-XOR* compressor.

#### Decompressor

- **`net_serf_qt_decompressor`**: Network-based decompressor using the *Serf-Qt* algorithm.
- **`net_serf_xor_decompressor`**: Network-based decompressor using the *Serf-XOR* algorithm.
- **`serf_qt_decompressor`**: Standard *Serf-Qt* decompression implementation.
- **`serf_xor_decompressor`**: Standard *Serf-XOR* decompression implementation.

#### Decompressor (32-bit)

- **`serf_qt_decompressor_32`**: 32-bit implementation of the *Serf-Qt* decompressor.
- **`serf_xor_decompressor_32`**: 32-bit implementation of the *Serf-XOR* decompressor.

#### Utils

- **`array`**: Utility functions for array operations.
- **`double`**: Utilities for handling double-precision floating-point numbers.
- **`elias_gamma_codec`**: Implementation of Elias Gamma encoding/decoding.
- **`float`**: Utilities for handling single-precision floating-point numbers.
- **`input_bit_stream`**: Handles input bit streams for reading compressed data.
- **`output_bit_stream`**: Handles output bit streams for writing compressed data.
- **`post_office_result`**: Data structures for post office problem results.
- **`post_office_solver`**: Solver for the post office problem (Adaptive encoding strategy).
- **`post_office_solver_32`**: 32-bit specific solver for the post office problem.
- **`serf_utils_32`**: 32-bit specific utility functions.
- **`serf_utils_64`**: 64-bit specific utility functions.
- **`zig_zag_codec`**: Implementation of Zig-Zag encoding/decoding.

### Tests

```
test
├── adjust_digit_calculator.cpp	# Lambda calculator
├── baselines					# Source code of all baselines
├── CMakeLists.txt				# Local CMake building configs
├── data_set					# Datasets
├── Perf_baseline_inc.hpp		# Including file of all baselines
├── Perf.cc						# Main benchmark program
├── Perf_expr_config.hpp		# Configs of experiments
├── Perf_expr_data_struct.hpp	# Data structures for storing intermediate results
├── Perf_file_utils.hpp			# Utils for benchmark program
├── reproduction_on_modelardb	# Reproduction of ModelarDB-related data
└── unit_test					# All unit tests
```

The names of all test suites in `Perf.cc` are consistent with those of the experiments in the paper.

## :snake:Python Interface

We design an interface for Python programs so that developers can use *Serf* in their applications.

More details refer to [Python Interface Documents](https://github.com/Spatio-Temporal-Lab/Serf/tree/new/pywrapper)

## :microscope:Reproduction

After completing the building process, you can get all data of experiments in our paper by running `PerformanceProgram`:

```bash
cd ./build/test
./PerformanceProgram
```

Data should be found in `{$source_code_dir}/test` with file name pattern `{$experiment_name}_{cr|ct|dt}_table.csv`. 

`cr` is the abbreviation for `compression ratio`;

`ct` is the abbreviation for `compression time`;

`dt` is the abbreviation for `decompression time`.

## :triangular_ruler:Code of Conduct

Refer to [Google C++ Style](https://google.github.io/styleguide/cppguide.html).

## :email:Contact

If you have any questions or need further assistance, feel free to reach out:

- [Xiaolong Xu](https://github.com/XXL2002) (Current Maintainer): xiaolong.xu [at] stu.cqu.edu.cn

- [Zechao Chen](blog.ackth.cc/about) (Previous Maintainer): zecc [at] acm.org

## :scroll:License

The project is licensed under the [Creative Commons "Non-Commercial" (CC BY-NC)](https://creativecommons.org/licenses/by-nc/4.0/).

## :paperclip:Paper & Reference

[Paper on ACM Digital Library](https://dl.acm.org/doi/10.1145/3725353)

```tex
@article{10.1145/3725353,
author = {Li, Ruiyuan and Chen, Zechao and Lu, Ruyun and Xu, Xiaolong and Yang, Guangchao and Chen, Chao and Bao, Jie and Zheng, Yu},
title = {Serf: Streaming Error-Bounded Floating-Point Compression},
year = {2025},
issue_date = {June 2025},
publisher = {Association for Computing Machinery},
address = {New York, NY, USA},
volume = {3},
number = {3},
url = {https://doi.org/10.1145/3725353},
doi = {10.1145/3725353},
journal = {Proc. ACM Manag. Data},
month = jun,
articleno = {216},
numpages = {27},
keywords = {error-bounded compression, floating-point compression, lossy compression, streaming compression}
}
```
