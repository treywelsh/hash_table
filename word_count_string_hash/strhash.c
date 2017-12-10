#include <assert.h>
#include <inttypes.h> /* PRIu32 */
#include <stdint.h> /* uintptr_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "strhash.h"
/* #include "strhash_priv.h"
 */

#define _MASK(a) ((a) - 1)
#define ROUNDUP(v, a) (((uintptr_t)(v) + _MASK(a)) & ~(_MASK(a)))

#define _bkt_null(b) ((b)->p == NULL)
#define _bkt_clean(b) sht_free((b)->p)

/*static inline int
 */
static int
_bkt_init(struct _sht_bucket *b, size_t blen) {
	size_t bkt_len;

	bkt_len = (blen) * sizeof(*b->p) + BKT_INIT_LEN;
	b->p = (char *)sht_malloc(bkt_len);
	if (b->p == NULL) {
		return SHTBKT_ERR_OOM;
	}

	memset(b->p, 0, bkt_len);
	b->len = bkt_len;
	b->used = 0;

	return SHT_SUCCESS;
}

/*static inline int
 */
static int
_bkt_grow(struct _sht_bucket *b, size_t b_required_len) {
	size_t b_newlen;
	char *b_newp;

	b_newlen = ROUNDUP((b_required_len), BKT_MIN_LEN);
	b_newp = (char *)sht_realloc(b->p, b_newlen);
	if (b_newp == NULL) {
		return SHTBKT_ERR_OOM;
	}

	memset(b_newp + b->len, 0, b_newlen - b->len);
	b->p = b_newp;
	b->len = b_newlen;
	return SHT_SUCCESS;
}


/*static inline int
 */
static int
_bkt_entry_append(struct _sht_bucket *b, const char *key,
	 sht_keylen_size_t keylen,
	 sht_wcount_t count) {
	size_t elm_len;
	sht_entry_t *elm_off;
	int retc = SHT_SUCCESS;

	assert(b != NULL);
	assert(key != NULL);
	assert(keylen > 0);

	elm_len = sht_entry_size(keylen);

	if (_bkt_null(b)) {
		retc = _bkt_init(b, elm_len);
	} else {
		if (elm_len + b->used >= b->len) {
			retc = _bkt_grow(b, elm_len + b->used);
		}
	}

	if (retc == SHT_SUCCESS) {
		elm_off = b->p + b->used;

        /* element copy */
		sht_keylen(elm_off) = keylen;
		memcpy(sht_keyp(elm_off), key, keylen);
		sht_data(elm_off) = count;

		b->used += elm_len;
	}

	return retc;
}

#define _sht_bkt_arr(arr, mask, i) (&(arr)[(i) & (mask)])
#define _sht_bkt(h, i) (_sht_bkt_arr((h)->bkt, (h)->bkt_mask, (i)))

int
sht_create(struct strhash **h, size_t bits) {
	assert(h != NULL);
	assert(bits > 0);

	*h = (struct strhash *)sht_malloc(sizeof(**h));
	if (*h == NULL) {
		return SHTBKT_ERR_OOM;
	}

	(*h)->elm_count = 0;
	(*h)->bkt_len = 1 << bits;
	(*h)->bkt_mask = (*h)->bkt_len - 1;

	(*h)->bkt = (struct _sht_bucket *)sht_malloc((*h)->bkt_len *
		sizeof(*(*h)->bkt));
	if ((*h)->bkt == NULL) {
		sht_free(*h);
		*h = NULL;
		return SHT_ERR_OOM_CREATE;
	}
	memset((*h)->bkt, 0, (*h)->bkt_len * sizeof(*(*h)->bkt));

	return SHT_SUCCESS;
}

void
sht_destroy(struct strhash *h) {
    size_t i;

	assert(h != NULL);

	for (i = 0; i < h->bkt_len; i++)
		if (!_bkt_null(h->bkt + i))
			_bkt_clean(h->bkt + i);
	sht_free(h->bkt);
	sht_free(h);
}

int
sht_lookup(struct strhash *h, const char *key, sht_keylen_size_t keylen,
	 sht_entry_t **ret) {
	struct _sht_bucket *b;
	sht_entry_t *e;

	assert(h != NULL);
	assert(key != NULL);
	assert(keylen > 0);
	assert(ret != NULL);

	*ret = NULL;
	if (keylen >= SHT_MAX_KEYLEN) {
		return SHT_ERR_KEY_TOO_LONG;
	}

	b = _sht_bkt(h, sht_hash(key, keylen));
	if (_bkt_null(b)) {
		return SHT_SUCCESS;
	}

	sht_bucket_foreach(b, e) {
		if (keylen == sht_keylen(e) && !memcmp(sht_keyp(e), key,
			 keylen)) {
			*ret = e;
			break;
		}
	}

	return SHT_SUCCESS;
}

/* allocate a new array of buckets twice larger
 * if sucessful
 *      foreach bucket from old array
 *          if bucket not null
 *              foreach word
 *                  get hash value
 *                  select new bucket
 *                  append entry
 *              free old bucket
 *      free old array of buckets
 */
/*static inline int
 */
static int
_sht_expand_buckets(struct strhash *h) {
	size_t new_hash;
	size_t bkts_new_len;
	size_t bkts_new_mask;
	struct _sht_bucket *b_new;
	struct _sht_bucket *bkts_new;
	size_t i;
	sht_entry_t *e;
	int _loop_retc = SHT_SUCCESS;
	int retc = SHT_SUCCESS;

	assert(h != NULL);

	/* allocate a new bucket array */
	bkts_new_len = h->bkt_len << 1;
	bkts_new_mask = bkts_new_len - 1;
	bkts_new = (struct _sht_bucket *)sht_malloc(bkts_new_len * sizeof(*h->bkt));
	if (bkts_new == NULL) {
		return SHT_ERR_OOM_EXPAND_BUCKETS;
	}
	memset(bkts_new, 0, bkts_new_len * sizeof(*h->bkt));

	for (i = 0; i < h->bkt_len; i++) {
		if (_bkt_null(h->bkt + i)) {
			continue;
		}
		/* for each element of the old bucket */
		sht_bucket_foreach(h->bkt + i, e) {
			new_hash = sht_hash(sht_keyp(e), sht_keylen(e));
			b_new = _sht_bkt_arr(bkts_new, bkts_new_mask, new_hash);

			/* try to append it in the new bucket */
			_loop_retc = _bkt_entry_append(b_new, sht_keyp(e),
				sht_keylen(e), sht_data(e));

			/* catch an OOM error in the loop to return an error code */
			if (_loop_retc != SHT_SUCCESS) {
				fprintf(stderr,
					 "error: not enough memory, word %.*s with count %" PRIu32 " lost\n",
					 sht_keylen(e), sht_keyp(e),  sht_data(e));
				retc = _loop_retc;
			}
		}
		_bkt_clean(h->bkt + i);
	}
	sht_free(h->bkt);
	h->bkt = bkts_new;
	h->bkt_len = bkts_new_len;
	h->bkt_mask = bkts_new_mask;

	return retc;
}

/* count a new element, and resize bucket array if needed
 *
 * if hash table contain too much elements
 *      expand bucket array
 * if good
 *      select bucket with old computed hash
 *      append entry
 */
/*static inline int
 */
static int
_sht_add_newelm(struct strhash *h, const char * key, sht_keylen_size_t keylen,
   sht_wcount_t data, size_t _hash) { 
        struct _sht_bucket *bkt;
        int retc = SHT_SUCCESS;

		h->elm_count += 1;
		if (h->elm_count > BKT_MAX_LOAD * h->bkt_len) {
			retc = _sht_expand_buckets(h);
		}
		if (retc == SHT_SUCCESS) {
			bkt = _sht_bkt(h, _hash);
			retc = _bkt_entry_append(bkt, key, keylen, data);
		}
    return retc;
}

/*
 * if keylen is ok
 *      compute hash and select bucket
 *      if bucket is null
 *          add element
 *      else
 *          old set to null
 *          loop and compare for each word of the bucket
 *              if found
 *                  return element and break loop
 *          if not found
 *              add element
 */
int
sht_tryadd(struct strhash *h, const char *key, sht_keylen_size_t keylen,
	 sht_wcount_t data, sht_entry_t **old) {
	struct _sht_bucket *b;
	sht_entry_t *e;
	size_t hash;
	int retc;

	assert(h != NULL);
	assert(key != NULL);
	assert(old != NULL);

	if (keylen >= SHT_MAX_KEYLEN) {
		return SHT_ERR_KEY_TOO_LONG;
	}
	retc = SHT_SUCCESS;
	hash = sht_hash(key, keylen);
	b = _sht_bkt(h, hash);
	if (_bkt_null(b)) {
		retc =_sht_add_newelm(h, key, keylen, data, hash);
	} else {
		*old = NULL;
		sht_bucket_foreach(b, e) {
			if (keylen == sht_keylen(e) && !memcmp(sht_keyp(e), key,
				 keylen)) {
				*old = e;
				break;
			}
		}
		if (*old == NULL) {
			retc = _sht_add_newelm(h, key, keylen, data, hash);
		}
	}

	return SHT_SUCCESS;
}

