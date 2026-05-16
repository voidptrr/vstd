#include <stdint.h>

#include "common/bench_payload.h"
#include "common/bench_report.h"
#include "cstd/datastruct/binary_heap.h"

#define CSTD_BENCH_WARMUP_OPS 50000ULL
#define CSTD_BENCH_MEASURED_OPS 5000000ULL
#define CSTD_BENCH_TRIALS 30U

static int cmp_payload_min(const void *a, const void *b) {
    const cstd_bench_payload *lhs = (const cstd_bench_payload *)a;
    const cstd_bench_payload *rhs = (const cstd_bench_payload *)b;

    if (lhs->id < rhs->id) {
        return -1;
    }
    if (lhs->id > rhs->id) {
        return 1;
    }
    return 0;
}

static cstd_status run_binary_heap_steady_push_pop_trial(uint64_t ops, double *ns_per_op) {
    cstd_binary_heap heap;
    if (cstd_binary_heap_init(&heap, sizeof(cstd_bench_payload), cmp_payload_min) != CSTD_OK) {
        return CSTD_ERR_STATE;
    }

    for (int i = 0; i < 16; i++) {
        cstd_bench_payload value = cstd_bench_payload_make((uint64_t)i);
        if (cstd_binary_heap_push(&heap, &value) != CSTD_OK) {
            cstd_binary_heap_free(&heap);
            return CSTD_ERR_STATE;
        }
    }

    uint64_t start = cstd_bench_now_ns();
    for (uint64_t i = 0; i < ops; i++) {
        cstd_bench_payload out;
        cstd_bench_payload value = cstd_bench_payload_make(i + 1000ULL);
        if (cstd_binary_heap_pop(&heap, &out) != CSTD_OK || cstd_binary_heap_push(&heap, &value) != CSTD_OK) {
            cstd_binary_heap_free(&heap);
            return CSTD_ERR_STATE;
        }
    }
    uint64_t end = cstd_bench_now_ns();

    cstd_binary_heap_free(&heap);
    *ns_per_op = (double)(end - start) / (double)ops;
    return CSTD_OK;
}

static cstd_status run_binary_heap_growth_push_pop_trial(uint64_t ops, double *ns_per_op) {
    cstd_binary_heap heap;
    if (cstd_binary_heap_init(&heap, sizeof(cstd_bench_payload), cmp_payload_min) != CSTD_OK) {
        return CSTD_ERR_STATE;
    }

    uint64_t start = cstd_bench_now_ns();
    for (uint64_t i = 0; i < ops; i++) {
        cstd_bench_payload value = cstd_bench_payload_make(i);
        if (cstd_binary_heap_push(&heap, &value) != CSTD_OK) {
            cstd_binary_heap_free(&heap);
            return CSTD_ERR_STATE;
        }

        if ((i & 1ULL) == 1ULL) {
            cstd_bench_payload out;
            if (cstd_binary_heap_pop(&heap, &out) != CSTD_OK) {
                cstd_binary_heap_free(&heap);
                return CSTD_ERR_STATE;
            }
        }
    }
    uint64_t end = cstd_bench_now_ns();

    cstd_binary_heap_free(&heap);
    *ns_per_op = (double)(end - start) / (double)ops;
    return CSTD_OK;
}

static cstd_status run_binary_heap_steady_peek_trial(uint64_t ops, double *ns_per_op) {
    cstd_binary_heap heap;
    if (cstd_binary_heap_init(&heap, sizeof(cstd_bench_payload), cmp_payload_min) != CSTD_OK) {
        return CSTD_ERR_STATE;
    }

    for (int i = 0; i < 128; i++) {
        cstd_bench_payload value = cstd_bench_payload_make((uint64_t)i);
        if (cstd_binary_heap_push(&heap, &value) != CSTD_OK) {
            cstd_binary_heap_free(&heap);
            return CSTD_ERR_STATE;
        }
    }

    uint64_t start = cstd_bench_now_ns();
    for (uint64_t i = 0; i < ops; i++) {
        cstd_bench_payload out;
        if (cstd_binary_heap_peek(&heap, &out) != CSTD_OK) {
            cstd_binary_heap_free(&heap);
            return CSTD_ERR_STATE;
        }
    }
    uint64_t end = cstd_bench_now_ns();

    cstd_binary_heap_free(&heap);
    *ns_per_op = (double)(end - start) / (double)ops;
    return CSTD_OK;
}

int main(void) {
    cstd_bench_case cases[] = {
        {.name = "steady_push_pop", .trial_fn = run_binary_heap_steady_push_pop_trial},
        {.name = "growth_push_pop", .trial_fn = run_binary_heap_growth_push_pop_trial},
        {.name = "steady_peek", .trial_fn = run_binary_heap_steady_peek_trial},
    };
    cstd_bench_result results[sizeof(cases) / sizeof(cases[0])];

    for (size_t i = 0; i < (sizeof(cases) / sizeof(cases[0])); i++) {
        if (cstd_bench_run_case(&cases[i], CSTD_BENCH_WARMUP_OPS, CSTD_BENCH_MEASURED_OPS,
                                CSTD_BENCH_TRIALS, &results[i]) != CSTD_OK) {
            return 1;
        }
    }

    cstd_bench_print_table("binary_heap", cases, results, sizeof(cases) / sizeof(cases[0]),
                           CSTD_BENCH_MEASURED_OPS, CSTD_BENCH_TRIALS);
    return 0;
}
