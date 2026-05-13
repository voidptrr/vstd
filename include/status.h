#ifndef CSTD_STATUS_H
#define CSTD_STATUS_H

typedef enum {
    CSTD_OK,
    CSTD_ERR_OOM,
    CSTD_ERR_NULL,
    CSTD_ERR_EMPTY,
    CSTD_ERR_RANGE,
    CSTD_ERR_STATE
} cstd_status;

#endif
