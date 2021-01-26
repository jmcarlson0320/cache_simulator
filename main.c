#include "defs.h"
#include <stdio.h>
#include <stdlib.h>

void usage(void)
{
    printf("usage: ./cache_sim <size> <block_size> <associativity> <trace_file>\n");
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        usage();
        return 0;
    }

    sim_state sim;
    init_sim(&sim, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));

    if (!load_trace(argv[4], &sim)) {
        return 0;
    }

    printf("*** running sim ***\n"
           "cache size (bytes): %d\n"
           "block size (bytes): %d\n"
           "%d-way set associativity\n"
           "trace file: %s\n",
           sim.cache_mem.capacity,
           sim.cache_mem.block_size,
           sim.cache_mem.set_associativity,
           argv[4]);

    trace_elem cur;
    while (next(&sim, &cur)) {
        process_trace_element(&sim, &cur);
    }

    // TODO more stats.
    // caclulate results
    float miss_rate = (float) sim.cache_misses / (float) sim.num_mem_refs;

    // print results
    printf("mem refs: %ld, hits: %ld, misses: %ld\n", sim.num_mem_refs, sim.cache_hits, sim.cache_misses);
    printf("miss rate: %f%% \n", miss_rate * 100.0);

    return 0;
}
