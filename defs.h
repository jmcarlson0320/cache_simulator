#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0
#define ERROR -1

typedef struct trace_elem trace_elem;
typedef struct cache_block cache_block;
typedef struct cache cache;
typedef struct sim_state sim_state;

struct trace_elem {
    int32_t microOpCount;
    int64_t instructionAddress;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
    int32_t destinationRegister;
    char conditionRegister;
    char TNnotBranch;
    char loadStore;
    int64_t immediate;
    int64_t addressForMemoryOp;
    int64_t fallthroughPC;
    int64_t targetAddressTakenBranch;
    char macroOperation[64];
    char microOperation[64];
};

struct cache_block {
    int valid;
    int64_t tag;
};

struct cache {
    int capacity; // bytes
    int block_size; // bytes
    int num_blocks;
    int set_associativity;
    int num_sets;
    int num_valid;
    cache_block **blocks; // rows = sets, columns = blocks in set
};

struct sim_state {
    FILE *trace_file;
    int64_t totalMicroops;
    int64_t totalMacroops;
    int64_t num_mem_refs;
    int64_t cache_hits;
    int64_t cache_misses;
    cache cache_mem;
};

void init_sim(sim_state *sim, int capacity, int block_size, int associativity);
int load_trace(char *file, sim_state *sim);
void print_element(trace_elem *e);
int next(sim_state *s, trace_elem *e);
void process_trace_element(sim_state *s, trace_elem *e);

#endif // DEFS_H
