#include <stdint.h>

#include "common/bench_payload.h"
#include "common/bench_report.h"
#include "cstd/compare.h"
#include "cstd/datastruct/hashmap.h"

#define CSTD_BENCH_WARMUP_OPS 50000ULL
#define CSTD_BENCH_MEASURED_OPS 5000000ULL
#define CSTD_BENCH_TRIALS 30U

static cstd_status run_hashmap_steady_put_get_trial(uint64_t ops, double *ns_per_op) {
    cstd_hashmap map;
    if (cstd_hashmap_init(&map, sizeof(uint64_t), sizeof(cstd_bench_payload), cstd_eq_u64) != CSTD_OK) {
        return CSTD_ERR_STATE;
    }

    for (uint64_t i = 0; i < 128U; i++) {
        cstd_bench_payload value = cstd_bench_payload_make(i);
        if (cstd_hashmap_put(&map, &i, &value) != CSTD_OK) {
            cstd_hashmap_free(&map);
            return CSTD_ERR_STATE;
        }
    }

    uint64_t start = cstd_bench_now_ns();
    for (uint64_t i = 0; i < ops; i++) {
        uint64_t key = i & 127U;
        cstd_bench_payload value = cstd_bench_payload_make(i + 1000ULL);
        cstd_bench_payload out;

        if (cstd_hashmap_put(&map, &key, &value) != CSTD_OK ||
            cstd_hashmap_get(&map, &key, &out) != CSTD_OK) {
            cstd_hashmap_free(&map);
            return CSTD_ERR_STATE;
        }
    }
    uint64_t end = cstd_bench_now_ns();

    cstd_hashmap_free(&map);
    *ns_per_op = (double)(end - start) / (double)ops;
    return CSTD_OK;
}

static cstd_status run_hashmap_growth_put_get_trial(uint64_t ops, double *ns_per_op) {
    cstd_hashmap map;
    if (cstd_hashmap_init(&map, sizeof(uint64_t), sizeof(cstd_bench_payload), cstd_eq_u64) != CSTD_OK) {
        return CSTD_ERR_STATE;
    }

    uint64_t start = cstd_bench_now_ns();
    for (uint64_t i = 0; i < ops; i++) {
        uint64_t key = i;
        cstd_bench_payload value = cstd_bench_payload_make(i);
        cstd_bench_payload out;

        if (cstd_hashmap_put(&map, &key, &value) != CSTD_OK) {
            cstd_hashmap_free(&map);
            return CSTD_ERR_STATE;
        }
        if ((i & 1ULL) == 1ULL) {
            uint64_t probe = i >> 1U;
            if (cstd_hashmap_get(&map, &probe, &out) != CSTD_OK) {
                cstd_hashmap_free(&map);
                return CSTD_ERR_STATE;
            }
        }
    }
    uint64_t end = cstd_bench_now_ns();

    cstd_hashmap_free(&map);
    *ns_per_op = (double)(end - start) / (double)ops;
    return CSTD_OK;
}

static cstd_status run_hashmap_steady_get_hit_trial(uint64_t ops, double *ns_per_op) {
    cstd_hashmap map;
    if (cstd_hashmap_init(&map, sizeof(uint64_t), sizeof(cstd_bench_payload), cstd_eq_u64) != CSTD_OK) {
        return CSTD_ERR_STATE;
    }

    for (uint64_t i = 0; i < 1024U; i++) {
        cstd_bench_payload value = cstd_bench_payload_make(i);
        if (cstd_hashmap_put(&map, &i, &value) != CSTD_OK) {
            cstd_hashmap_free(&map);
            return CSTD_ERR_STATE;
        }
    }

    uint64_t start = cstd_bench_now_ns();
    for (uint64_t i = 0; i < ops; i++) {
        uint64_t key = i & 1023U;
        cstd_bench_payload out;
        if (cstd_hashmap_get(&map, &key, &out) != CSTD_OK) {
            cstd_hashmap_free(&map);
            return CSTD_ERR_STATE;
        }
    }
    uint64_t end = cstd_bench_now_ns();

    cstd_hashmap_free(&map);
    *ns_per_op = (double)(end - start) / (double)ops;
    return CSTD_OK;
}

int main(void) {
    cstd_bench_case cases[] = {
        {.name = "steady_put_get", .trial_fn = run_hashmap_steady_put_get_trial},
        {.name = "growth_put_get", .trial_fn = run_hashmap_growth_put_get_trial},
        {.name = "steady_get_hit", .trial_fn = run_hashmap_steady_get_hit_trial},
    };
    cstd_bench_result results[sizeof(cases) / sizeof(cases[0])];

    for (size_t i = 0; i < (sizeof(cases) / sizeof(cases[0])); i++) {
        if (cstd_bench_run_case(&cases[i], CSTD_BENCH_WARMUP_OPS, CSTD_BENCH_MEASURED_OPS,
                                CSTD_BENCH_TRIALS, &results[i]) != CSTD_OK) {
            return 1;
        }
    }

    cstd_bench_print_table("hashmap", cases, results, sizeof(cases) / sizeof(cases[0]),
                           CSTD_BENCH_MEASURED_OPS, CSTD_BENCH_TRIALS);
    return 0;
}
