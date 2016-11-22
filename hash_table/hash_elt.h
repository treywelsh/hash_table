#ifndef HASH_ELT_H_
#define HASH_ELT_H_

#include <assert.h>
#include <sys/queue.h>
#include <string.h>

/* For each new usage of this implementation of hash table,
 * this header has to be redefined.
 */

/*
 * Key
 * Must be defined :
 *  - ht_key_init
 *  - ht_key_clean
 *  - ht_key_reset
 *  - ht_key_cmp
 *  - ht_hash
 */

struct ht_key {
    char * word;
    size_t len;
};
typedef struct ht_key ht_key_t;

static inline int
ht_key_init(ht_key_t * k, const char * word, size_t len) {

    assert(k != NULL);

    k->word = strndup(word, len);
    if (k->word == NULL) {
        return 1;
    }
    k->len = len;

    return 0;
}
#define ht_key_clean(k) free((k)->word)
#define ht_key_reset(k) memset((k)->word, '\0', (k)->len)

static inline int
ht_key_cmp(const ht_key_t * e1, const ht_key_t * e2) {
    assert(e1 != NULL);
    assert(e1->word != NULL);
    assert(e1->len > 0);

    assert(e2 != NULL);
    assert(e2->word != NULL);
    assert(e2->len > 0);

    if (e1->len != e2->len) {
        return 1;
    }
    return strncmp(e1->word, e2->word, e1->len);
}


/*
 * Value
 * Must be defined :
 *  - ht_value_init
 *  - ht_value_clean
 *  - ht_value_reset
 *  - ht_if_key_equal
 */

struct ht_value {
    unsigned int count;
};
typedef struct ht_value ht_value_t;

#define ht_value_init(v) ((v)->count = 0)
#define ht_value_clean (void)
#define ht_value_reset ht_value_init

/* This function is called when an element
 * is already present in hashtable.
 * In this case, it permits to the user to act on
 * the element.
 * For instance, it can be use if we want count 
 * occurences of a word in a text.
 */
static inline int
ht_if_key_equal(ht_value_t * v) {
    (v->count)++;
    return 0;
}

/* Jenkins one at a time hash algorithm */
static inline size_t
ht_hash(const ht_key_t * k) {
    size_t hash;
    size_t i;

    assert(k != NULL);
    assert(k->word != NULL);

    hash = 0;
    for (i = 0; i < k->len; i++)
    {
        hash += (k->word)[i]; /* differ signedness*/
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

#endif /* HASH_ELT_H_ */
