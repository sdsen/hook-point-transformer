#ifndef __BALANCER_HELPERS
#define __BALANCER_HELPERS
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <stdbool.h>
#include <linux/pkt_cls.h>
#include <linux/pkt_cls.h>
#include <linux/pkt_cls.h>
#include "balancer_consts.h"
#include "balancer_structs.h"
#include "bpf.h"
#include "bpf_helpers.h"
#include "control_data_maps.h"
#include "csum_helpers.h"
#include "introspection.h"
#define bpf_printk(fmt, ...)                                   \
  ({                                                           \
    char ____fmt[] = fmt;                                      \
    bpf_trace_printk(____fmt, sizeof(____fmt), ##__VA_ARGS__); \
  })
#ifdef KATRAN_INTROSPECTION

__attribute__ ((__always_inline__)) static inline void submit_event (struct __sk_buff *ctx, void *map, __u32 event_id, void *data, __u32 size, bool metadata_only) {
    struct ctl_value *gk;
    __u32 introspection_gk_pos = 5;
    gk = bpf_map_lookup_elem (&ctl_array, &introspection_gk_pos);
    if (!gk || gk->value == 0) {
        return;
    }
    struct event_metadata md = {};
    __u64 flags = BPF_F_CURRENT_CPU;
    md.event = event_id;
    md.pkt_size = size;
    if (metadata_only) {
        md.data_len = 0;
    }
    else {
        md.data_len = min_helper (size, MAX_EVENT_SIZE);
        flags |= (__u64) md.data_len << 32;
    }
    bpf_perf_event_output (ctx, map, flags, & md, sizeof (struct event_metadata));
}
#endif
#ifdef INLINE_DECAP_GENERIC

__attribute__ ((__always_inline__)) static inline int recirculate (struct __sk_buff *ctx) {
    int i = RECIRCULATION_INDEX;
    bpf_tail_call (ctx, & subprograms, i);
    return TC_ACT_OK;
}
#endif // of INLINE_DECAP_GENERIC

__attribute__ ((__always_inline__)) static inline int decrement_ttl (void *data, void *data_end, int offset, bool is_ipv6) {
    struct iphdr *iph;
    struct ipv6hdr *ip6h;
    if (is_ipv6) {
        if ((data + offset + sizeof (struct ipv6hdr)) > data_end) {
            return TC_ACT_SHOT;
        }
        ip6h = (struct ipv6hdr *) (data + offset);
        if (!--ip6h->hop_limit) {
            return TC_ACT_SHOT;
        }
    }
    else {
        if ((data + offset + sizeof (struct iphdr)) > data_end) {
            return TC_ACT_SHOT;
        }
        iph = (struct iphdr *) (data + offset);
        __u32 csum;
        if (!--iph->ttl) {
            return TC_ACT_SHOT;
        }
        csum = iph->check + 0x0001;
        iph->check = (csum & 0xffff) + (csum >> 16);
    }
    return FURTHER_PROCESSING;
}
#endif

