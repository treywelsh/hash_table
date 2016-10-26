#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "errc.h"
#include "hash.h"

int
main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    ht_key_t key;
    int ret;
    ht_t ht;
    ht_elt_t *result;
    size_t i;
    size_t len;
    const char * strings[] = {
        "aaa",
        "bbb",
        "ccc",
        "ddd",
        "eeeEEE",
        "aaa",
        "fff",
        "ggg",
        "hhh"
    };
    size_t strings_count = 8;

    ret = ht_init(&ht, 10, 4);
    if (ret != SUCCESS) {
        return ret;
    }

    /* fill hash from strings */
    for (i = 0; i < strings_count; i++) {
        len = strlen(strings[i]);

        printf("\nadding \"%s\" len=%zu\n", strings[i], len);

        ht_key_init(&key, strings[i], len);

        result = ht_lookup(&ht, &key);
        assert(result != NULL);

        /* modify value, it's a counter here */
        (ht_get_value(result)->count)++;
        printf("%s present with value %" PRIu64 "\n",
                strings[i], ht_get_value(result)->count);

        /* display lru in order to check if it's works */
        printf("=======================LRU=====================\n");
        ht_print_lru_content(&ht);
        printf("===============================================\n");
    }

    ht_clean(&ht);

    return 0;
}
