#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "hash_elt.h"
#include "hash.h"


int
ht_init(ht_t *ht, size_t two_pow_size) {
    size_t i;
    size_t size;

    assert(ht != NULL && two_pow_size > 0);

    /* hash table size */
    size = (1 << two_pow_size);

    ht->heads_elt = malloc((size) * sizeof(*ht->heads_elt));
    if (ht->heads_elt == NULL) {
        return HT_MALLOC_ERROR;
    }

    for (i = 0; i < size ; i++) {
        SLIST_INIT(&(ht->heads_elt)[i]);
    }

    ht->size = size;
    /* ht->entries_count = 0; */

    return SUCCESS;
}

void
ht_clean(ht_t * ht) {
    size_t i;
    ht_elt_t * first;
    assert(ht != NULL);

    /* For each head of */
    for (i = 0 ; i < ht->size ; i++) {
        while(!SLIST_EMPTY(&(ht->heads_elt)[i])) {
            first = SLIST_FIRST(&(ht->heads_elt)[i]);
            SLIST_REMOVE_HEAD(&(ht->heads_elt)[i], next);
            ht_elt_destroy(first);
        }
    }

    free(ht->heads_elt);
}

ht_elt_t *
ht_lookup(const ht_t * ht, const ht_key_t * k) {
    size_t hash;
    ht_elt_t *p;

    hash = ht_hash(k);
    SLIST_FOREACH(p, &(ht->heads_elt)[hash & (ht->size - 1)], next) {
        /* compare keys */
        if (!ht_elt_cmp(&p->key, k)) {
            return p;
        }
    }
    return NULL;
}

int
ht_add(ht_t * ht, ht_elt_t * e) {
    size_t hash;
    ht_elt_t *p;

    assert(ht != NULL);
    assert(e != NULL);
    assert((e->key).word != NULL);
    assert((e->key).len > 0);

    hash = ht_hash(&e->key);
    SLIST_FOREACH(p, &(ht->heads_elt)[hash & (ht->size - 1)], next) {
        /* compare keys */
        if (!ht_elt_cmp(&p->key, &e->key)) {
            /* If already in hash, act on
	     * already inserted element
	     */
	    ht_elt_already_added(e);
            return 1;
        }
    }
    SLIST_INSERT_HEAD(&(ht->heads_elt)[hash & (ht->size - 1)], e, next);
    return SUCCESS;
}

int
ht_remove(ht_t * ht, const ht_key_t * k) {
    size_t hash;
    ht_elt_t *p;

    hash = ht_hash(k);

    SLIST_FOREACH(p, &(ht->heads_elt)[hash & (ht->size - 1)], next) {
        /* compare keys */
        if (!ht_elt_cmp(&p->key, k)) {
            SLIST_REMOVE(&(ht->heads_elt)[hash & (ht->size - 1)], p, ht_elt, next);
            ht_elt_destroy(p);
            break;
        }
    }

    /* TODO add return values when found or not */
    return SUCCESS;
}
