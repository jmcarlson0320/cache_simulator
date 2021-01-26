#include "defs.h"
#include <stdio.h>

void usage(void)
{
    printf("usage: ./cache_sim trace_file\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        usage();
        return 0;
    }

    sim_state sim;
    init_sim(&sim);

    if (!load_trace(argv[1], &sim)) {
        return 0;
    }

    trace_elem cur;
    while (next(&sim, &cur)) {
        process_trace_element(&sim, &cur);
    }

    printf("mem refs: %ld, hits: %ld, misses: %ld\n", sim.num_mem_refs, sim.cache_hits, sim.cache_misses);

    float hit_rate = (float) sim.cache_hits / (float) sim.num_mem_refs;

    printf("hit rate: %f%% \n", hit_rate * 100.0);

    return 0;
}
