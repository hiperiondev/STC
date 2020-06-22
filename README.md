STC - C99 Standard Container Library
====================================

Introduction
------------

An elegant, typesafe, generic, customizable, user-friendly, consistent, and very efficient standard container library for C99. This is a small headers only library with the most used container components, and a few algorithms:
- **cstring.h** - Compact and powerful **string** class.
- **cvector.h** - Dynamic generic **vector** class.
- **chash.h** - Unordered **map** and **set** implemented as open hashing without tombstones. Highly customizable and fast.
- **carray.h** - Multi-dimensional dynamic **array**, implemented as a single contiguous section of memory.
- **clist.h** - A circular singly linked **list**, can be used as a **queue** (supports O(1) *pushBack, pushFront, and popFront*). Also contains various *splice* functions and (merge) *sorting*.
- **coption.h** - Implementation of *getopt_long*-"like" function, *coption_get*, to parse command line arguments.
- **crandom.h** - Collection of some efficent modern random number generators *xoroshiro128ss*, *sfc32/64* and Mersenne Twister *mt19937*. It also implements the crypto-strong *siphash* algorithm.
- **cdefs.h** - A very small common include file with som central definitions.

The usage of the containers is similar to the C++ standard containers, so it should be easier for those who are familiar with them.

All containers mentioned above, except for CString are generic (similar to templates in C++). A simple example:
```
#include <stc/vector.h>
declare_CVector(i, int);

int main(void) {
    CVector_i vec = cvector_init;
    cvector_i_pushBack(&vec, 42);
    cvector_i_destroy(&vec);
}
```
Motivation
----------

The goal of this project was to finally create a **Standard Container Library for the C language**. I suspect that most earlier attempts at this has failed because they did not meet one or several of the following requirements. A standard container library should
- be easy to use, intuitive naming and consistency across the library.
- be type safe. Minimal usage of casting and void* pointers.
- be highly efficient. Both in speed and memory usage.
- be customizable without losing efficiency. E.g. inline replacable compare, hash, allocation functions per container type instantiation.
- have a fairly small code base, and easy to install, deploy and maintain.
- avoid bloat. It should not try to cover all thinkable functions, but limit itself to the most useful and commonly used.

That said, this library is far from complete or free of possible bugs, but I believe it is a good foundation.

Installation
------------

Because it is headers only, files can simply be included in your program. The functions will be inlined by default. If containers are extensively used accross many tranlation units with common instantiated container types, it is recommended to build as a library, to minimize executable size. In this case, specify **-DSTC_HEADER** as compiler argument, and put all the instantiations of the containers used in one C file, e.g.
```
#define STC_IMPLEMENTATION
#include <stc/cvector.h>
#include <stc/chash.h>

declare_CVector(i, int);
declare_CHash(ii, map, int, int);
declare_CHash(64, set, int64_t);
...
```
Performance
-----------

This library is very efficent. Containers have templated intrusive elements. One of the most performance critical containers is the **CHash map / CHash set**. Thankfully, is it among the fastest C / C++ map implementations: **examples/benchmark.c** compiled with g++ v9.2.0 -O3 on windows (the results are similar with VC++ and g++ on linux):

**CMAP**=*CHash map*, KMAP=*khash*, UMAP=*std::unordered_map*, BMAP=*ska::bytell_hash_map*, FMAP=*ska::flat_hash_map*, RMAP=*robin_hood::unordered_map*
```
Random keys are in range [0, 2^20):
map<uint64_t, uint64_t>: 7000000 repeats of Insert random key + (try to) remove a different random key:
CMAP(ii): sz: 523938, bucks: 1013337, time: 0.39, sum: 24500003500000, erase: 3237392 --> fastest
KMAP(ii): sz: 523938, bucks: 2097152, time: 0.46, sum: 24500003500000, erase: 3237392
UMAP(ii): sz: 523938, bucks: 1056323, time: 2.21, sum: 24500003500000, erase: 3237392
BMAP(ii): sz: 523938, bucks: 1048576, time: 0.46, sum: 24500003500000, erase: 3237392
FMAP(ii): sz: 523938, bucks: 1048576, time: 0.43, sum: 24500003500000, erase: 3237392
RMAP(ii): sz: 523938, bucks: 838860, time: 0.82, sum: 24500003500000, erase: 3237392

map<uint64_t, uint64_t>: Insert 10000000 sequensial keys, then remove them in same order:
CMAP(ii): sz: 0, bucks: 17001171, time: 0.75, erase 10000000 --> second fastest
KMAP(ii): sz: 0, bucks: 16777216, time: 0.48, erase 10000000
UMAP(ii): sz: 0, bucks: 17961079, time: 1.04, erase 10000000
BMAP(ii): sz: 0, bucks: 16777216, time: 1.04, erase 10000000
FMAP(ii): sz: 0, bucks: 16777216, time: 0.94, erase 10000000
RMAP(ii): sz: 0, bucks: 13421772, time: 0.84, erase 10000000

map<uint64_t, uint64_t>: Insert 10000000 random keys, then remove them in same order:
CMAP(ii): sz: 0, bucks: 1621347, time: 0.41, erase 1048490 --> fastest
KMAP(ii): sz: 0, bucks: 2097152, time: 0.77, erase 1048490
UMAP(ii): sz: 0, bucks: 2144977, time: 1.67, erase 1048490
BMAP(ii): sz: 0, bucks: 2097152, time: 0.52, erase 1048490
FMAP(ii): sz: 0, bucks: 2097152, time: 0.44, erase 1048490
RMAP(ii): sz: 0, bucks: 1677721, time: 0.65, erase 1048490
```
Memory efficiency
-----------------

The containers are memory efficent, i.e. they occupy as little memory as practical possible.
- **CString**, **CVector**: Representaion: one pointer size. The size and capacity is stored as part of the heap allocation that also holds the vector elements.
- **CList**: Representation: one pointer size. Each node allocates block storing value and next pointer.
- **CHash set**: Representation: 4 pointers size. The hash table stores a key per bucket, and one table of "used/hash-value", occupying only one byte per bucket.
- **CHash map**: Same as CHash set, but each bucket in the array stores a (key, value) pair, not only the key.
- **CArray**: Elements are stored as one memory block. Representation: Two pointers, plus variables to store dimensionality.

CHash and CVector discussion
----------------------------

**CHash** is the most complex of the containers (although, currently only ~370 lines of code). You can customize the destroy-, hash- and equals- function. In addition it supports a few other arguments in the declare-statement that allows to define a convertion from a raw/literal type to the key-type specified. This is handy when e.g. having CString as key, as it enables us to use string literals as key in put() and get() functions, instead of a constructed CString. Without it you would need to write: 
```
chash_si_put(&map, cstring_make("mykey"), 12);
```
but even worse:
```
CString lookup = cstring_make("mykey");
int x = chash_si_get(&map, lookup)->value;
cstring_destroy(&lookup);
```
The predefined shorthand macro *declare_CHash_string()* defines a CHash container with a CString as key, however you may use it like:
```
chash_si_put(&map, "mykey", 12);
int x = chash_si_get(&map, "mykey")->value; // no allocation of string key happens here.
```
An alternative would be to use *char* * as key type, but you would the need to manage memory of the hash string keys yourself.
Note that this customization is also available for **CVector**, but only affects the *find()* function currently. See *declare_CVector_string()*.

Demos
-----
The first example has a complex nested container type, which demonstrates some of the capability of the library. Look at the simpler examples below to understand it better. The example adds an element into the data structure, and then accesses it. The type used, with c++ template syntax is:
**CHashMap**< **CString**, **CHashMap**< *int*, **CList**< **CArray2**< *float* >>>>

Note that *chash_sm_destroy(&theMap)* call below will also destroy all the nested containers, including memory allocated for CString keys in theMap object.
```
#include "stc/cstring.h"
#include "stc/chash.h"
#include "stc/clist.h"
#include "stc/carray.h"

void check_destroy(float* v) {printf("destroy %g\n", *v);}

declare_CArray(f, float, check_destroy);
declare_CList(t2, CArray2_f, carray2_f_destroy, c_noCompare);
declare_CHash(il, MAP, int, CList_t2, clist_t2_destroy);
declare_CHash_string(sm, MAP, CHash_lm, chash_lm_destroy);

int main() {
    int dim1 = 4, dim2 = 6;
    CHash_sm theMap = chash_init;
    {
        // Construct.
        CArray2_f table = carray2_f_make(dim1, dim2, 0.f);
        CList_t2 tableList = clist_init;
        CHash_il listMap = chash_init;
        
        // Put in some data.
        carray2_f_data(table, 2)[5] = 3.1415927; // table[2][5]
        clist_t2_pushBack(&tableList, table);
        chash_il_put(&listMap, 42, tableList);
        chash_sm_put(&theMap, "First", listMap);
    }

    // Access the data entry
    CArray2_f table = clist_back(chash_il_get(&chash_sm_get(&theMap, "First")->value, 42)->value);
    printf("value is: %f\n", carray2_f_value(table, 3, 5));

    chash_sm_destroy(&theMap); // free up the whole shebang!
}
```
**CString**
```
#include <stc/cstring.h>

int main() {
    CString s1 = cstring_make("one-nine-three-seven-five");
    printf("%s.\n", s1.str);

    cstring_insert(&s1, 3, "-two");
    printf("%s.\n", s1.str);

    cstring_erase(&s1, 7, 5); // -nine
    printf("%s.\n", s1.str);

    cstring_replace(&s1, 0, "seven", "four");
    printf("%s.\n", s1.str);
    printf("find: %s\n", s1.str + cstring_find(s1, 0, "four"));

    // reassign:
    cstring_assign(&s1, "one two three four five six seven");
    cstring_append(&s1, " eight");
    printf("append: %s\n", s1.str);
    cstring_destroy(&s1);

    CString s2 = cstring_makeFmt("Index %d: %f", 123, 4.56);
    cstring_destroy(&s2);
}
```
**CVector** of *int64_t*
```
#include <stc/cvector.h>
declare_CVector(ix, int64_t); // ix is just an example tag name, use anything without underscore.

int main() {
    CVector_ix bignums = cvector_init; // = (CVector_ix) cvector_init; if initializing after declaration.
    cvector_ix_reserve(&bignums, 100);
    for (size_t i = 0; i<100; ++i)
        cvector_ix_pushBack(&bignums, i * i * i);
    cvector_ix_popBack(&bignums); // erase the last

    uint64_t value;
    for (size_t i = 0; i < cvector_size(bignums); ++i)
        value = bignums.data[i];
    cvector_ix_destroy(&bignums);
}
```
**CVector** of *CString*
```
#include <stc/cstring.h>
#include <stc/cvector.h>
declare_CVector_string(cs);

int main() {
    CVector_cs names = cvector_init;
    cvector_cs_pushBack(&names, cstring_make("Mary"));
    cvector_cs_pushBack(&names, cstring_make("Joe"));
    cstring_assign(&names.data[1], cstring_make("Jake")); // replace Joe

    printf("%s\n", names.data[1].str); // Access the string char*
    cvector_cs_destroy(&names);
}
```
**CHash map** of *int -> int*
```
#include <stc/chash.h>
declare_CHash(ii, MAP, int, int);

int main() {
    CHash_ii nums = chash_init;
    chash_ii_put(&nums, 8, 64);
    chash_ii_put(&nums, 11, 121);

    printf("%d\n", chash_ii_get(nums, 8)->value);
    chash_ii_destroy(&nums);
}
```
**CHash set** of *CString*
```
#include <stc/cstring.h>
#include <stc/chash.h>
declare_CHash_string(s, SET); // See the discussion above regarding this declaration.

int main() {
    CHash_s words = chash_init;
    chash_s_put(&words, "Hello");
    chash_s_put(&words, "Groovy");
    chash_s_erase(&words, "Hello");

    // iterate the map:
    c_foreach (i, chash_s, words)
        printf("%s\n", i.item->key.str);
    chash_s_destroy(&words);
}
```
**CHash map** of *CString -> CString*. Temporary CString values are created by *cstring_make()*, and moved into the container
```
#include <stc/cstring.h>
#include <stc/chash.h>
declare_CHash_string(ss, MAP, CString, cstring_destroy); 

int main() {
    CHash_ss table = chash_init;
    chash_ss_put(&table, "Make", cstring_make("my"));
    chash_ss_put(&table, "Sunny", cstring_make("day"));
    printf("Sunny: %s\n", chash_ss_get(table, "Sunny")->value.str);
    chash_ss_erase(&table, "Make");

    printf("size %d\n", chash_size(table));
    chash_ss_destroy(&table); // frees key and value CStrings, and hash table (CVector).
}
```
**CList** of *int64_t*. Similar to c++ *std::forward_list*, but can do both *pushFront()* and *pushBack()*.
```
    #include <stc/clist.h>
    #include <stc/crandom.h>
    declare_CList(i, int64_t);
 
    int main() {
        CList_i list = clist_init;
        int n;
        sfc64_t rng = sfc64_seed(1234);
        for (int i=0; i<1000000; ++i) // one million random numbers
            clist_i_pushBack(&list, sfc64_rand(&rng));
        n = 0; 
        c_foreach (i, clist_i, list)
            if (++n % 10000 == 0) printf("%d: %zd\n", n, i.item->value);
        // Sort them...
        clist_i_sort(&list); // mergesort O(n*log n)
        n = 0;
        c_foreach (i, clist_i, list)
            if (++n % 10000 == 0) printf("%d: %zd\n", n, i.item->value);
        clist_i_destroy(&list);
    }
```
**CArray**. Heap allocated 1D, 2D and 3D array in one memory block, with sub-arrays.
```
#include <stc/carray.h>
declare_CArray(f, float);

int main()
{
    CArray3_f a3 = carray3_f_make(30, 20, 10, 0.f);
    carray3_f_data(a3, 5, 4)[3] = 10.2f;  // a3[5][4][3]
    CArray2_f a2 = carray3_f_at(a3, 5);   // sub-array reference (no data copy).

    printf("%f\n", carray2_f_value(a2, 4, 3));   // readonly lookup a2[4][3] (=10.2f)
    printf("%f\n", carray2_f_data(a2, 4)[3]);    // same, but this is writable.
    printf("%f\n", carray2_f_at(a2, 4).data[3]); // same, via sub-array access.
    
    printf("%f\n", carray3_f_value(a3, 5, 4, 3)); // same data location, via a3 array.
    printf("%f\n", carray3_f_data(a3, 5, 4)[3]);
    printf("%f\n", carray3_f_at2(a3, 5, 4).data[3]);
    
    carray2_f_destroy(&a2); // does nothing, since it is a sub-array.
    carray3_f_destroy(&a3); // also invalidates a2.
}
```
