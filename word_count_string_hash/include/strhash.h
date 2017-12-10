#ifndef STRHASH_H_
#define STRHASH_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
/* #include <limits.h> UCHAR_MAX */

/* #include "strhash_priv.h" */

#define sht_malloc malloc
#define sht_realloc realloc
#define sht_free free
#define sht_hash _sht_oat

/* types used in an entry */
/* typedef _sht_entry_t        sht_entry_t;
 */
typedef uint8_t  sht_keylen_size_t;
#define SHT_MAX_KEYLEN 255 

typedef uint32_t sht_wcount_t;
typedef char sht_entry_t;

/* Jenkins one at a time hashing algorithm */
/*static inline size_t 
 */
static size_t 
_sht_oat(const char *k, sht_wcount_t klen) {
	size_t hash = 0;
	sht_wcount_t i;

	for (i = 0; i < klen; i++) {
		hash += k[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

#pragma pack(push, 1)
struct _bkt_entry {
    sht_wcount_t data;
    sht_keylen_size_t keylen;
    char key[sizeof(int) - sizeof(sht_keylen_size_t)];
};
#pragma pack(pop)

typedef union _bkt_entry_u {
    char *c;
    struct _bkt_entry *e;
}_bkt_entry_u_t;

struct _sht_bucket {
    sht_entry_t *p;
	size_t len;
	size_t used;
};

/* Elements accessors
 * /!\ you have to use it on each values returned by sht_lookup and sht_tryadd /!\
 */
#define sht_keyp(p)     (((_bkt_entry_u_t)(p)).c + sizeof(struct _bkt_entry))
#define sht_entry(p)    (((_bkt_entry_u_t)(p)).e)
#define sht_keylen(p)   (sht_entry((p))->keylen)
#define sht_data(p)     (sht_entry((p))->data)

#define sht_entry_size(klen) (sizeof(struct _bkt_entry) + (klen))
#define sht_next_entryp(p) ((p) + sht_entry_size(sht_keylen((p))))

/* Configured with arbitrary values */
#define BKT_INIT_LEN (5 * CACHELINE_LEN) /* The size of the bucket when a first word
	                                      * is inserted (buckets are not initialized at start)
	                                      */
#define BKT_MIN_LEN (2 * CACHELINE_LEN) /* The reallocated size of the bucket
	                                     * is larger than asked, aligned on a multiple of this value
	                                     */
#define BKT_MAX_LOAD 5 /* max fill factor per bucket, beyond this limit,
	                    *  the bucket array is reallocated twice larger.
	                    */

struct strhash {
	struct _sht_bucket *bkt;
	size_t bkt_len;
	size_t bkt_mask;
	sht_wcount_t elm_count;
};

int sht_create(struct strhash **h, size_t bits);
void sht_destroy(struct strhash *h);
int sht_lookup(struct strhash *h, const char *key, sht_keylen_size_t keylen,
     sht_entry_t **ret);

/* Add an element if there is no collision
 * else, return the element adress in old */
int sht_tryadd(struct strhash *h, const char *word, sht_keylen_size_t wordlen,
     sht_wcount_t data, sht_entry_t **ret_old);

#define sht_bucket_foreach(b, e) \
	for ((e) = (b)->p; (e) < (b)->p + (b)->used; (e) = sht_next_entryp((e)))
#define sht_foreach(h, e) \
	for (size_t _sht_i = 0; _sht_i < (h)->bkt_len; _sht_i++) \
		sht_bucket_foreach((h)->bkt + _sht_i, (e))


#endif /* STRHASH_H_ */
