#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "hash_elt.h"
#include "hash.h"


int
ht_init(ht_t *ht, size_t two_pow_size) {
	size_t i;

	assert(ht != NULL && two_pow_size > 0);

	/* hash table size */
	ht->heads_max = 1 << two_pow_size;
	ht->heads_mask = ht->heads_max - 1;

	ht->heads = malloc(ht->heads_max * sizeof(*ht->heads));
	if (ht->heads == NULL) {
		return HT_MALLOC_ERROR;
	}

	for (i = 0; i < ht->heads_max; i++) {
		SLIST_INIT(&(ht->heads)[i]);
	}

	/* ht->entries_count = 0; */

	return SUCCESS;
}

void
ht_clean(ht_t *ht) {
	size_t i;
	ht_elt_t *first;
	assert(ht != NULL);

	/* For each head of */
	for (i = 0; i < ht->heads_max; i++) {
		while (!SLIST_EMPTY(&ht->heads[i])) {
			first = SLIST_FIRST(&ht->heads[i]);
			SLIST_REMOVE_HEAD(&ht->heads[i], next);
			ht_elt_destroy(first);
		}
	}

	free(ht->heads);
}

static inline ht_elt_t *
_ht_lookup_hash(const ht_t *ht, const ht_key_t *key, size_t hash) {
	ht_elt_t *p;

	assert(ht != NULL);
	assert(key != NULL);

	SLIST_FOREACH(p, &(ht->heads)[hash & ht->heads_mask], next) {
		if (!ht_key_cmp(ht_get_key(p), key)) {
			/* if equal, return element */
			return p;
		}
	}
	return NULL;
}

ht_elt_t *
ht_lookup(const ht_t *ht, const ht_key_t *k) {
	ht_elt_t *ret;

	ret = _ht_lookup_hash(ht, k, ht_hash(k));
	if (ret != NULL) {
		return ret;
	}
	return NULL;
}

int
ht_add(ht_t *ht, ht_elt_t *e) {
	size_t hash;
	ht_elt_t *ret;
	ht_key_t *k;

	assert(ht != NULL);
	assert(e != NULL);

	k = ht_get_key(e);

	assert(k->word != NULL);
	assert(k->len > 0);

	hash = ht_hash(k);
	ret = _ht_lookup_hash(ht, k, hash);
	if (ret != NULL) {
		/* If already in hash, act on
		 * previously inserted value element
		 */
		ht_if_key_equal(ht_get_value(ret));

		return HT_ALREADY_ADDED;
	}

	/* insert new element */
	SLIST_INSERT_HEAD(&(ht->heads)[hash & ht->heads_mask], e, next);

	return SUCCESS;
}

int
ht_remove(ht_t *ht, const ht_key_t *k) {
	size_t hash;
	ht_elt_t *ret;

	hash = ht_hash(k);
	ret = _ht_lookup_hash(ht, k, ht_hash(k)); \
	if (ret != NULL) {
		SLIST_REMOVE(&(ht->heads)[hash & ht->heads_mask], ret, ht_elt, next);
		ht_elt_destroy(ret);
		return SUCCESS;
	}

	return ERROR;
}
