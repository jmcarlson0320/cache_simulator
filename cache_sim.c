#include "defs.h"
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

void init_sim(sim_state *sim, int capacity, int block_size, int associativity)
{
    sim->cache_mem.capacity = capacity;
    sim->cache_mem.block_size = block_size;
    sim->cache_mem.num_blocks = capacity / block_size;
    sim->cache_mem.set_associativity = associativity;
    sim->cache_mem.num_sets = sim->cache_mem.num_blocks / associativity;
    sim->cache_mem.num_valid = 0;
    
    // allocate cache
    sim->cache_mem.blocks = malloc(sim->cache_mem.num_sets * sizeof(cache_block *));
    for (int i = 0; i < sim->cache_mem.num_sets; i++) {
        sim->cache_mem.blocks[i] = malloc(sim->cache_mem.set_associativity * sizeof(cache_block));
    }

    // initialize cache
    for (int j = 0; j < sim->cache_mem.num_sets; j++) {
        for (int i = 0; i < sim->cache_mem.set_associativity; i++) {
            sim->cache_mem.blocks[j][i].tag = 0;
            sim->cache_mem.blocks[j][i].valid = 0;
        }
    }

    sim->totalMicroops = 0;
    sim->totalMacroops = 0;
    sim->cache_hits = 0;
    sim->cache_misses = 0;
    sim->num_mem_refs = 0;

    srand(time(0));
}

int load_trace(char *file, sim_state *sim)
{
    int success = TRUE;
    sim->trace_file = fopen(file, "r");

    if (!sim->trace_file) {
        printf("could not open file %s\n", file);
        success = FALSE;
    }

    printf("opened file %s\n", file);

    return success;
}

void print_element(trace_elem *e)
{
    printf("%x, ", e->microOpCount);
    printf("%lx, ", e->instructionAddress);
    printf("%d, ", e->sourceRegister1);
    printf("%d, ", e->sourceRegister2);
    printf("%d, ", e->destinationRegister);
    printf("%c, ", e->conditionRegister);
    printf("%c, ", e->TNnotBranch);
    printf("%c, ", e->loadStore);
    printf("%ld, ", e->immediate);
    printf("%lx, ", e->addressForMemoryOp);
    printf("%lx, ", e->fallthroughPC);
    printf("%lx, ", e->targetAddressTakenBranch);
    printf("%s, ", e->macroOperation);
    printf("%s\n", e->microOperation);
}

int next(sim_state *s, trace_elem *e)
{
    int success = TRUE;

    int result = fscanf(s->trace_file, 
                        "%" SCNi32
                        "%" SCNx64 
                        "%" SCNi32
                        "%" SCNi32
                        "%" SCNi32
                        " %c"
                        " %c"
                        " %c"
                        "%" SCNi64
                        "%" SCNx64
                        "%" SCNx64
                        "%" SCNx64
                        "%11s"
                        "%22s\n",
                        &e->microOpCount,
                        &e->instructionAddress,
                        &e->sourceRegister1,
                        &e->sourceRegister2,
                        &e->destinationRegister,
                        &e->conditionRegister,
                        &e->TNnotBranch,
                        &e->loadStore,
                        &e->immediate,
                        &e->addressForMemoryOp,
                        &e->fallthroughPC,
                        &e->targetAddressTakenBranch,
                        e->macroOperation,
                        e->microOperation);
                        
    if (result == EOF) {
        success = FALSE;
    } else if (result != 14) {
        fprintf(stderr, "Error parsing trace at line %" PRIi64 "\n", s->totalMicroops);
        success = FALSE;
    }

    return success;
}

void process_trace_element(sim_state *s, trace_elem *e)
{
    s->totalMicroops++;

    if (e->microOpCount == 1) {
        s->totalMacroops++;
    }

    int64_t addr = e->addressForMemoryOp;

    if (addr == 0)
        return;

    s->num_mem_refs++;

    // truncate for cache block size
    // truncate for set associativity
    // mod for number of cache blocks
    // extract tag
    // index into cache
    // compare tag to each block in associated set
    // if full
    //      determine which entry to remove
    // add tag to cache
    // update valid flag

    int64_t block_addr = (addr / s->cache_mem.block_size) % s->cache_mem.num_blocks;
    int64_t tag = addr / (s->cache_mem.block_size * s->cache_mem.num_blocks);
    int64_t set_number = block_addr % s->cache_mem.num_sets;

    // see if we have a hit
    int hit = 0;
    for (int i = 0; i < s->cache_mem.set_associativity; i++) {
        if (tag == s->cache_mem.blocks[set_number][i].tag &&
            s->cache_mem.blocks[set_number][i].valid == 1) {
            hit = 1;
            s->cache_hits++;
            break;
        }
    }

    // no hit, find an invalid block to replace
    int block_added = 0;
    if (!hit) {
        s->cache_misses++;
        for (int i = 0; i < s->cache_mem.set_associativity; i++) {
            if (s->cache_mem.blocks[set_number][i].valid == 0) {
                s->cache_mem.blocks[set_number][i].tag = tag;
                s->cache_mem.blocks[set_number][i].valid = 1;
                block_added = 1;
                s->cache_mem.num_valid++;
                break;
            }
        }
    }

    // no invalid blocks in set, randomly replace a valid one
    if (!block_added) {
        int block_number = rand() % 2;
        s->cache_mem.blocks[set_number][block_number].tag = tag;
    }

//    printf("%ld\tmem ref: %lx\t%d\n", s->totalMicroops, e->addressForMemoryOp, s->cache_mem.num_valid);
}
