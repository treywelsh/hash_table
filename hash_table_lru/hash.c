#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "err.h"
#include "errc.h"
#include "hash_elt.h"
#include "hash.h"

int
ht_init(ht_t *ht, size_t ht_pow_size, size_t pool_max) {
    size_t i;

    assert(ht != NULL);
    assert(ht_pow_size > 0);

    /* hash table size */
    ht->heads_max = (1 << ht_pow_size);

    /* Initialize hash table : array of linked lists */
    ht->heads = malloc(ht->heads_max * sizeof(*ht->heads));
    if (ht->heads == NULL) {
        return HT_MALLOC_ERROR;
    }
    for (i = 0; i < ht->heads_max ; i++) {
        LIST_INIT(&(ht->heads)[i]);
    }
    /* Allocate pool of elements */
    ht->pool = calloc(pool_max, sizeof(struct ht_elt));
    if (ht->pool == NULL) {
        return HT_MALLOC_ERROR;
    }

    /* Initialize LRU linked list */
    ht->lru_head = malloc(sizeof(*ht->lru_head));
    if (ht->lru_head == NULL) {
       return HT_MALLOC_ERROR;
    }
    TAILQ_INIT(ht->lru_head);

    /* Fill LRU with pool elements */
    for (i = 0 ; i < pool_max ; i++) {
        TAILQ_INSERT_HEAD(ht->lru_head, &(ht->pool)[i], next_lru);
    }

    ht->pool_max = pool_max;

    return SUCCESS;
}

void
ht_clean(ht_t * ht) {
    size_t i;

    assert(ht != NULL);

    /* Remove hash elements */
    for (i = 0 ; i < ht->heads_max ; i++) {
        while(!LIST_EMPTY(&(ht->heads)[i])) {
            LIST_REMOVE(LIST_FIRST(&(ht->heads)[i]), next);
        }
    }

    free(ht->heads);
    free(ht->lru_head);
    free(ht->pool);
}

static inline ht_elt_t *
_ht_lookup_hash(const ht_t * ht, const ht_key_t * key, size_t hash) {
    ht_elt_t *p;

    assert(ht != NULL);
    assert(key != NULL);

    LIST_FOREACH(p, &(ht->heads)[hash & (ht->heads_max - 1)], next) {
        if (!ht_key_cmp(&(p->key), key)) {
            return p;
        }
    }
    return NULL;
}

ht_elt_t *
ht_lookup(ht_t * ht, const ht_key_t * key) {
    size_t hash;
    size_t old_hash;
    ht_elt_t *ret;
    ht_elt_t *elt;

    assert(ht != NULL);
    assert(key != NULL);

    hash = ht_hash(key);
    ret = _ht_lookup_hash(ht, key, hash);
    if (ret != NULL) {
        /* If key is already in hash table :
         * element is moved to the end of lru
         */
        TAILQ_REMOVE(ht->lru_head, ret, next_lru);
        TAILQ_INSERT_TAIL(ht->lru_head, ret, next_lru);
        return ret;
    }

    /* If it's a new element, we take the least recent used element
     * in order to recycling it */
    elt = TAILQ_FIRST(ht->lru_head);

    /* move it at end of lru */
    TAILQ_REMOVE(ht->lru_head, elt, next_lru);
    TAILQ_INSERT_TAIL(ht->lru_head, elt, next_lru);

    /* Search the position of this (maybe) old hash element
     * in order to remove it */
    old_hash = ht_hash(&(elt->key));
    ret = _ht_lookup_hash(ht, &(elt->key), old_hash);
    if (ret != NULL) {
        assert(elt == ret);
        LIST_REMOVE(ret, next);
    }

    /* reset element with new data then
     * insert it in hash */
    ht_elt_reset(elt);
    ht_key_cpy(&(elt->key), key);
    LIST_INSERT_HEAD(&(ht->heads)[hash & (ht->heads_max - 1)], elt, next);

    return elt;
}

void
ht_print_lru_content(const ht_t * ht) {
    ht_elt_t *p;
    /* size_t i; */

    assert(ht != NULL);

    TAILQ_FOREACH(p, ht->lru_head, next_lru) {
        printf("%s\n", (p->key).data);
        /* print bytes
           for (i = 0; i < DATA_LEN; i++) {
           printf("%02x ", ((p->key).data)[i]);
           }
           printf("\n");
           */
    }
}
