#ifndef WC_FILE_H_
#define WC_FILE_H_

#include <stdint.h>
#include <stdio.h>

struct wordcount {
	struct strhash *h;
	uint64_t tot_words;
};

int wc_init(struct wordcount *wc);
void wc_clean(struct wordcount *wc);

int wc_file(const char *fname, struct wordcount *wc);
int wc_stream(FILE *fp, struct wordcount *wc);
int wc_dictfile(const char *fname, struct wordcount *wc);

void wc_print(const struct wordcount *wc);


#endif /* WC_FILE_H_ */
