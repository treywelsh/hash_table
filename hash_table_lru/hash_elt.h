#ifndef HASH_ELT_H_
#define HASH_ELT_H_

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/queue.h>

#include "err.h"
#include "errc.h"

#define DATA_LEN 3

struct ht_key {
     uint8_t data[DATA_LEN];
     uint8_t padding[2]; /* remove warning */
};
typedef struct ht_key ht_key_t;

#define ht_key_init(key, src_data, src_data_len) do { \
    assert((src_data_len) == DATA_LEN); \
    memcpy((key)->data, (src_data), DATA_LEN); \
}while(0)

#define ht_key_reset(k) \
    memset((k)->data, 0, DATA_LEN)

#define ht_key_cpy(d, s) \
    memcpy((d)->data, (s)->data, DATA_LEN)

#define ht_key_cmp(k1, k2) \
    memcmp((k1)->data, (k2)->data, DATA_LEN)

/* Jenkins one at a time hash */
static inline size_t
ht_hash(const ht_key_t * k) {
    size_t hash;
    size_t i;

    assert(k != NULL);

    hash = 0;
    for (i = 0; i < DATA_LEN ; i++)
    {
        hash += (k->data)[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

/* hash table value */
struct ht_value {
    uint64_t count;
};
typedef struct ht_value ht_value_t;

#define ht_value_reset(v) \
    ((v)->count = 0)

#endif /* HASH_ELT_H_ */
