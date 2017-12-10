# Word count with string hash table

Why an hash table for a word count ?
It's simple, and easy to implement.

It's a cache efficient implementation of a string hash table.
It tries to reduce the use of pointers in order to avoid cache misses and a better use of cache prefetching.
By the way, this implementation should also be more memory efficient (we don't have
to store the pointers to chain the elements).

A bucket is a dynamic array of bytes in which we store the elements contigous.
The counterpart: it implies heavy memory copy when resizing buckets (realloc).

It's inspired of N. Askitis work.

I suppose a clean dictfile and input files:
 - a word : at least one character, all aphanumeric
 - only one word per line in dictfile
 - comments in dictfile have '#' as first character (" #" will considered as a word)
 - word len : I store the word len in an uint16_t. It can be modified for bigger words.
 - no useless spaces on input (it will be considered as a part of word)

There is no remove operation because we don't need it for a wordcount.

## Build
make clean ; make
or
make re

For debug purpose :
make clean ; make DEBUG=1
or
make re DEBUG=1

You can make a little functional test with :
make test

## Portability and robustness

1. Memory alignement : I don't enforce memory alignement in my hash table implementation.
2. I use getline function, so I have to define the feature macro : _POSIX_C_SOURCE 200809L
from man: (Since glibc 2.10) The value 200809L or greater exposes definitions
corresponding to the POSIX.1-2008 base specification (excluding the XSI extension).
3. I use some c99 stuffs as inline keyword, or variable declaration in for loop.
5. I don't check all integers limit with macros from limits.h
6. Not fully tested with an allocator returning NULL sometimes

## Possible improvements
 - Test scripts
 - choose the best hash function for the key domain.
    My implementation use One At a Time function from Jenkins by default.
 - have a deeper look at efficient tries implementations as the HAT trie
 - multithread
 - maybe some bloom filters ?

### Further readings
 - Nikolas Askitis thesis on cache efficient data structures : http://bit.ly/2AMja42
 --> Show some specialized data structure (hash, tries and others) cache and memory efficient
