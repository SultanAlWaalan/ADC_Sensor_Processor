# ADC Sensor Log Processor

## Project Overview

This project is a command-line C program.

The program reads a binary ADC sensor log file, validates the file header, loads ADC sample records into dynamically allocated memory, converts raw 12-bit ADC readings into voltage values, computes per-channel statistics, detects fault conditions, checks sequence number integrity, and writes structured output reports.

The project was developed using CLion with CMake and the C99 standard.

## Project Context

An Analogue-to-Digital Converter, or ADC, converts real-world analogue signals such as voltage into digital integer values that a computer program can process.

The dataset represents a 4-channel, 12-bit ADC system sampled at 1000 Hz. Each raw ADC reading is converted into voltage using:

```c
voltage = (raw_value / 4095.0) * 3.3;
```

The value `4095.0` is used because a 12-bit ADC has 4096 possible levels, from 0 to 4095. Floating-point division is required to avoid integer division errors.

## Program Features

The program performs the following tasks:

- Reads and validates the 24-byte binary file header
- Checks the magic number `0xADC1BEEF`
- Dynamically allocates memory based on the record count
- Reads 4000 ADC sample records from the binary file
- Converts raw ADC values to voltage
- Computes per-channel statistics:
    - Mean voltage
    - Minimum voltage
    - Maximum voltage
    - Standard deviation
- Detects fault conditions:
    - Overvoltage: voltage greater than 3.0 V
    - Undervoltage: voltage less than 0.3 V
    - Sensor fault flag
    - Out-of-range flag
- Checks sequence number integrity
- Writes a summary report to `results.txt`
- Writes a detailed fault report to `fault_report.txt`

## Project File Structure

```text
ADC_Project/
     main.c
     adc.c
     adc.h
     io.c
     io.h
     stats.c
     stats.h
     CMakeLists.txt
     README.md
     adc_sensor_log.bin
     results.txt
     fault_report.txt
```

## Source File Responsibilities

### main.c

Contains the main program flow only. It reads the command-line argument, calls the file-reading function, calls the analysis functions, writes the output reports, frees allocated memory, and exits.

### adc.c / adc.h

Contains ADC-specific data structures and analysis functions, including voltage conversion, per-channel statistics, fault detection, and sequence gap detection.

### io.c / io.h

Contains all file input and output logic. This includes reading the binary ADC file, validating the header, writing `results.txt`, and writing `fault_report.txt`.

### stats.c / stats.h

Contains reusable statistical functions for mean, minimum, maximum, and standard deviation.

### CMakeLists.txt

Defines the CMake build configuration and ensures the project is compiled using the C99 standard.

## How to Build and Run in CLion

1. Open CLion.
2. Select **Open Project**.
3. Choose the `ADC_Project` folder.
4. Allow CLion to load the CMake project.
5. Make sure `adc_sensor_log.bin` is in the project folder.
6. Go to **Run then Edit Configurations**.
7. Set the program argument to:
    ```text
      adc_sensor_log.bin
    ```
8. Set the working directory to the main project folder.
9. Click **Apply**, then **OK**.
10. Click **Run**.

Expected console output:

```text
Analysis complete.
Summary written to results.txt
Fault report written to fault_report.txt
```

## How to Build and Run Using gcc

From the project folder, run:

```bash
gcc -std=c99 -Wall -Wextra -o adc_processor main.c adc.c io.c stats.c -lm
```

Then run:

```bash
./adc_processor adc_sensor_log.bin
```

For windows, run:

```bash
adc_processor.exe adc_sensor_log.bin
```

## Output Files

### results.txt

This file contains the main structured analysis report, including:

- Header information
- Per-channel statistics
- Fault counts
- Sequence integrity findings
- Engineering interpretation

### fault_report.txt

This file contains a detailed fault log. Each faulted record is listed with:

- Record number
- Timestamp
- Channel
- Raw ADC value
- Converted voltage
- Sequence number
- Fault type

A single record may appear more than once if it contains multiple fault types. For example, a status flag value of `0x03` means both bit 0 and bit 1 are set, so the record has both a sensor fault and an out-of-range condition.

## GitHub Repository

Repository link:

```text
https://github.com/SultanAlWaalan/ADC_Sensor_Processor
```

## Notes on Memory Management

The program uses `malloc()` to allocate memory for ADC samples based on the `record_count` value read from the file header. The allocated memory is released using `free()` before the program exits.

## Notes on Binary Parsing

The binary file is read using `fread()`. Packed structs are used for the binary header and binary sample record so that the C compiler does not insert hidden padding bytes between fields. This is important because the file format has fixed byte sizes.

## Extension Implemented

The project includes the Fault Report File extension.

The summary report gives fault counts, while `fault_report.txt` gives detailed traceability for every faulted record. This is useful in engineering because it shows exactly when each fault occurred and which channel caused it.
