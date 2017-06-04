# hash_table

It's a non thread-safe implementation of an hash table.

How to use ?
Redefine key, value structures, and associated macros in hash_elt.h.
This file contain all the behaviour around hash table elements used
internally in hash table sources.

Why use a TAILQ list ?
1. need to append at end for the sentinel
2. no need to run the linked list to remove

Example:
...

For a thread safe implem, some requirements :
- currently, only one sentinel is used in the hash table
- ...
