#ifndef __DECAP_MAPS_H
#define __DECAP_MAPS_H
#include "bpf.h"
#include "bpf_helpers.h"
#ifndef STATS_MAP_SIZE
#define STATS_MAP_SIZE 4
#endif
struct decap_stats {
    __u64 decap_v4;
    __u64 decap_v6;
    __u64 total;
};
struct {
    __uint (type, BPF_MAP_TYPE_PERCPU_ARRAY);
    __type (key, __u32);
    __type (value, struct decap_stats);
    __uint (max_entries, STATS_MAP_SIZE);
    __uint (map_flags, NO_FLAGS);
} decap_counters SEC (".maps");
#endif // of _DECAP_MAPS
