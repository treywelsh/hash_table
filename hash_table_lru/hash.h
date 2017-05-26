#ifndef HASH_H_
#define HASH_H_

#include <stdint.h>
#include <sys/queue.h>

#include "hash_elt.h"

/* Hastable entry */
struct ht_elt {
	LIST_ENTRY(ht_elt) next;
	TAILQ_ENTRY(ht_elt) next_lru;

	/* key */
	ht_key_t key;

	/* value */
	ht_value_t value;
};
typedef struct ht_elt ht_elt_t;

#define ht_get_key(e) (&((e)->key))
#define ht_get_value(e) (&((e)->value))
#define ht_elt_reset(e) do { \
		ht_key_reset(&(e)->key); \
		ht_value_reset(&(e)->value); \
} while (0)

/* Hastable */
struct ht {
	ht_elt_t *pool;
	size_t pool_max;

	/* Hash */
	LIST_HEAD(, ht_elt) *heads;
	size_t heads_max;

	/* LRU */
	TAILQ_HEAD(, ht_elt) *lru_head;
};
typedef struct ht ht_t;

/* hash table size : size = 2^pow_size
 * elt_max : pool size */
int ht_init(ht_t *h, size_t ht_pow_size, size_t pool_max);
void ht_clean(ht_t *h);

/* look for a key, if not found,
 * return least recent used element */
ht_elt_t * ht_lookup(ht_t *h, const ht_key_t *key);

/* print LRU in order to debug */
void ht_print_lru_content(const ht_t *h);

#endif /* HASH_H_ */
