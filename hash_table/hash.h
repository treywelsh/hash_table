#ifndef HASH_H_
#define HASH_H_

#include <sys/queue.h>
#include <string.h>

#include "hash_elt.h"

/* Return codes */
enum {
    SUCCESS = 0,
    HT_ALREADY_ADDED,
    HT_MALLOC_ERROR,
    HT_ERROR,
};

/* Hastable */
struct ht {
    SLIST_HEAD(ht_head, ht_elt) *heads_elt;
    size_t size;
    /* size_t entries_count; number of inserted elements in hash */
};
typedef struct ht ht_t;

int ht_init(ht_t * h, size_t two_pow_size); /* Size is defined as follow :
					       size = 2^two_pow_size */
void ht_clean(ht_t * h);

ht_elt_t * ht_lookup(const ht_t * h, const ht_key_t *key);
int ht_add(ht_t * h, ht_elt_t * e);
int ht_remove(ht_t * h, const ht_key_t *key);

#endif /* HASH_H_ */
