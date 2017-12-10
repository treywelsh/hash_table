#include <stdio.h>

#include "err.h"
#include "wc_file.h"

/*static inline void*/
static void
usage(const char *pname) {
	fprintf(stderr,
		 "USAGE: %s dictfile [files ...]\n\nIf no files parameters, read on stdin\n",
		 pname);
}

int
main(int argc, char **argv) {
	int i, retc;
	struct wordcount wc;

	if ((retc = wc_init(&wc) != WC_SUCCESS)) {
		fprintf(stderr, "error <wc_init>: retcode=%d\n", retc);
	}

	if (argc < 2) {
		usage(argv[0]);
		retc = WC_ERR_ARGS;
		goto clean;
	} else if (argc == 2) {
		if ((retc = wc_dictfile(argv[1], &wc)) != WC_SUCCESS) {
			goto clean;
		}
		retc = wc_stream(stdin, &wc);
	} else {
		if ((retc = wc_dictfile(argv[1], &wc)) != WC_SUCCESS) {
			goto clean;
		}

		for (i = 2; i < argc; i++) {
			retc = wc_file(argv[i], &wc);
		}
	}

	wc_print(&wc);

clean:
	wc_clean(&wc);

	return retc;
}
