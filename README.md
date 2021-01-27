# Homework 3 Cache Simulator

Joshua Carlson  
CS431 Intro to Performance  
Professor K. Karanovic  
January 26, 2021  

This program simulates the performance of an n-way cache. It takes in a trace file and a
cache configuration, and outputs the hits/misses and the miss rate.

## Installation

To build, run make on the included Makefile

```bash
make
```

## Usage

```bash
cache_sim <cache size (bytes)> <block size (bytes)> <n-way associativity> <tracefile.trace>
```
