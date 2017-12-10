#include <errno.h>
#include <inttypes.h> /* PRIu32, PRIu64 */
#include <stdio.h>

#include "err.h"
#include "strhash.h"
#include "wc_file.h"


/*static inline void _wc_line(const char *buf, size_t bufread, struct
 */
static void _wc_line(const char *buf, size_t bufread, struct
     wordcount *wc);

#define HT_BUCKET_BITS 5 /* 2^HT_BUCKET_BITS is the number of buckets used */

int
wc_init(struct wordcount *wc) {
	wc->tot_words = 0;

	return sht_create(&(wc->h), HT_BUCKET_BITS);
}

void
wc_clean(struct wordcount *wc) {
	sht_destroy(wc->h);
}

void
wc_print(const struct wordcount *wc) {
	sht_entry_t *e;

	sht_foreach(wc->h, e) {
		fprintf(stdout, "%.*s\t%" PRIu32 "\n", sht_keylen(e), sht_keyp(e),
			 sht_data(e));
	}

	fprintf(stdout, "total words\t%" PRIu64 "\n", wc->tot_words);

}

int
wc_stream(FILE *fp, struct wordcount *wc) {
	ssize_t bufread;
	char *buf = NULL;
	size_t buflen = 0;
	int retc = WC_SUCCESS;

	while ((bufread = getline(&buf, &buflen, fp)) > 0) {
		_wc_line(buf, (size_t)(bufread - 1), wc);
	}
	if (!feof(fp) && bufread == -1) {
		fprintf(stderr, "error <getline>: %s\n", strerror(errno));
		retc = WC_ERR_GETLINE;
	}

	if (buf)
		free(buf);

	return retc;
}

int
wc_file(const char *fname, struct wordcount *wc) {
	FILE *fp;

	fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stderr, "error <fopen>: input file \"%s\" : %s\n", fname,
			 strerror(errno));
		return WC_ERR_FOPEN;
	}

	wc_stream(fp, wc);
	fclose(fp);

	return WC_SUCCESS;
}

/*static inline void
 */
static void
_wc_line(const char *buf, size_t bufread, struct wordcount *wc) {
	size_t w_off;
    sht_keylen_size_t w_len;
	sht_entry_t *ret = NULL;

	w_len = 0;
	w_off = 0;
	while (w_off < bufread) {

		/* get next word len */
		while (w_off + w_len < bufread && buf[w_off + w_len] != ' ') {
			w_len++;
		}

		wc->tot_words += 1;
		sht_lookup(wc->h, buf + w_off, (w_len ), &ret);
		if (ret != NULL) {
			sht_data(ret)++;
		}

		/* shift to next word */
		w_off += w_len + 1; /* +1 for the space between words */
		w_len = 0;
	}
}

int
wc_dictfile(const char *fname, struct wordcount *wc) {
	FILE *fp;
	char *buf = NULL;
	size_t buflen = 0;
	ssize_t bufread;
	sht_entry_t *old;
	size_t retc = SHT_SUCCESS;

	fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stderr, "error <fopen>: dictionary file \"%s\" : %s\n", fname,
			 strerror(errno));
		return WC_ERR_FOPEN;
	}

	while ((bufread = getline(&buf, &buflen, fp)) > 0) {
		if (buf[0] == '#' || buf[0] == '\n') {
			continue;
		}
		retc = sht_tryadd(wc->h, buf, (sht_keylen_size_t)(bufread - 1), 0, &old);
		if (retc != SHT_SUCCESS) {
			fprintf(stderr, "warning: words are dropped, retcode :%zu\n", retc);
		}
	}
	if (!feof(fp) && bufread == -1) {
		fprintf(stderr, "error: getline %s\n", strerror(errno));
		retc = WC_ERR_GETLINE;
	}


	if (buf != NULL)
		free(buf);

	fclose(fp);

	return retc;
}
