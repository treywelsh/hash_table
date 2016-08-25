#include <stdio.h>
#include <stdlib.h>

#include "hash.h"


int
main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int ret;
    ht_t ht;
    ht_elt_t *e;
    ht_elt_t *result_lookup;
    ht_key_t k;
    const char * filename = "/var/log/messages";

    ret = ht_init(&ht, 14);
    if (ret != SUCCESS ) {
        return ret;
    }

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("error: cannnot open %s\n", filename);
        exit(1);
    }

    /* Each line of the file is inserted in hash */
    while ((read = getline(&line, &len, fp)) != -1) {

        printf("adding \"%s\" len=%zu\n", line, read);

        e = ht_elt_create(line, read);
        if (e == NULL) {
            printf("error: element creation failed\n");
            ht_elt_destroy(e);
            exit(1);
        }

        ret = ht_add(&ht, e);
        if (ret == HT_ALREADY_ADDED) {
            printf("\"%s\" already added in hash\n", line);

            /* Element is user defined.
	     * Here, it contains the count of the word when added several times.
	     */
            result_lookup = ht_lookup(&ht, &e->key);
            if (result_lookup != NULL) {
                printf("found with count %u\n", result_lookup->count);
            }

            ht_elt_destroy(e);
        }
    }

    fclose(fp);
    if (line)
        free(line);

    k.word = "the";
    k.len = 4;

    printf("Adding %s\n", k.word);
    e = ht_elt_create(k.word, k.len);
    ret = ht_add(&ht, e);
    if (ret == HT_ALREADY_ADDED) {
        printf("\"%s\" already added in hash\n", k.word);
        ht_elt_destroy(e);
    }

    e = ht_lookup(&ht, &k);
    if (e == NULL) {
        printf("\"%s\" not found\n", k.word);
        exit(1);
    }
    printf("\"%s\" found with count %u\n", (e->key).word, e->count);

    printf("Removing %s\n", k.word);
    ht_remove(&ht, &k);

    e = ht_lookup(&ht, &k);
    if (e == NULL) {
        printf("\"%s\" not found, it has been removed with success!\n", k.word);
    }

    ht_clean(&ht);

    return 0;
}
