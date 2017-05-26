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
	ERROR,
};

/* Hastable entry */
struct ht_elt {
	SLIST_ENTRY(ht_elt) next;

	ht_key_t key;
	ht_value_t value;
};
typedef struct ht_elt ht_elt_t;

#define ht_get_key(e) ( & (e)->key)
#define ht_get_value(e) ( & (e)->value)

static inline ht_elt_t *
ht_elt_create(const char *word, size_t len) {
	ht_elt_t *e;

	e = malloc(sizeof(*e));
	if (e == NULL) {
		return NULL;
	}
	ht_key_init(ht_get_key(e), word, len);
	ht_value_init(ht_get_value(e));

	return e;
}

static inline void
ht_elt_destroy(ht_elt_t *e) {

	assert(e != NULL);

	ht_key_clean(ht_get_key(e));
	ht_value_clean(ht_get_value(e));

	free(e);
}

/* Hastable */
struct ht {
	SLIST_HEAD(ht_head, ht_elt) *heads;
	size_t heads_max;
	size_t heads_mask;
};
typedef struct ht ht_t;

int ht_init(ht_t *h, size_t pow_size);  /* hash table size = 2^pow_size */
void ht_clean(ht_t *h);

int ht_add(ht_t *h, ht_elt_t *e);
int ht_remove(ht_t *h, const ht_key_t *key);

ht_elt_t * ht_lookup(const ht_t *h, const ht_key_t *key);

#endif /* HASH_H_ */
