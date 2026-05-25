#ifndef CK_COMMON_PANIC_H
#define CK_COMMON_PANIC_H

_Noreturn void ck_panic(const char *message);

#define CK_ASSERT(cond, message)                                                                   \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            ck_panic(message);                                                                     \
        }                                                                                          \
    } while (0)

#endif
