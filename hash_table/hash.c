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
		TAILQ_INIT(&(ht->heads)[i]);
	}

	/* ht->entries_count = 0; */

	return SUCCESS;
}

void
ht_clean(ht_t *ht) {
	size_t i;
	ht_elt_t *first;
	assert(ht != NULL);

	/* For each list */
	for (i = 0; i < ht->heads_max; i++) {
		while (!TAILQ_EMPTY(&ht->heads[i])) {
			first = TAILQ_FIRST(&ht->heads[i]);
			TAILQ_REMOVE(&ht->heads[i], first, next);
			ht_elt_destroy(first);
		}
	}

	free(ht->heads);
}

static inline ht_elt_t *
_ht_lookup_hash(const ht_t *ht, const ht_key_t *key, size_t hash) {
	ht_elt_t *p;
	ht_elt_t stnl;

	assert(ht != NULL);
	assert(key != NULL);

	ht_key(&stnl) = *key;
	TAILQ_INSERT_TAIL(&(ht->heads)[hash & ht->heads_mask], &stnl, next);

	/* XXX  A sentinel node can be used here */
	//TAILQ_FOREACH(p, &(ht->heads)[hash & ht->heads_mask], next) {
	for (p = TAILQ_FIRST(&(ht->heads[hash & ht->heads_mask]))
	     ; ht_key_cmp(&ht_key(p), key)
	     ; p = TAILQ_NEXT(p, next)) {
	}
	TAILQ_REMOVE(&(ht->heads)[hash & ht->heads_mask], &stnl, next);

	if (&ht_key(p) != &ht_key(&stnl)) {
		/* if not the sentinel, we've found it */
		return p;
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

	k = &ht_key(e);

	assert(k->word != NULL);
	assert(k->len > 0);

	hash = ht_hash(k);
	ret = _ht_lookup_hash(ht, k, hash);
	if (ret != NULL) {
		/* If already in hash, act on
		 * previously inserted value element
		 */
		ht_if_key_equal(&ht_value(ret));

		return HT_ALREADY_ADDED;
	}

	/* insert new element */
	TAILQ_INSERT_HEAD(&(ht->heads)[hash & ht->heads_mask], e, next);

	return SUCCESS;
}

int
ht_remove(ht_t *ht, const ht_key_t *k) {
	ht_elt_t *ret;
	size_t hash = ht_hash(k);

	ret = _ht_lookup_hash(ht, k, hash);
	if (ret != NULL) {
		TAILQ_REMOVE(&(ht->heads)[hash & ht->heads_mask], ret, next);
		ht_elt_destroy(ret);
		return SUCCESS;
	}

	return ERROR;
}
