/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef CLIST__H__
#define CLIST__H__

#include <stdlib.h>
#include "cdefs.h"

/*  Circular Singly-linked Lists.
    
    This implements a std::forward_list-like class in C, but because it is circular,
    it also support push* and splice* at both ends of the list. This makes it ideal
    for being used as a queue, unlike std::forward_list. Basic usage is similar to CVec:
   
    #include <stdio.h>
    #include <stc/clist.h>
    #include <stc/crandom.h>
    declare_clist(ix, int64_t);
 
    int main() {
        clist_ix list = clist_init;
        crandom32_t pcg = crandom32_uniform_engine(12345);
        int n;
        for (int i=0; i<1000000; ++i) // one million
            clist_ix_pushBack(&list, crandom32_uniform_int(&pcg));
        n = 0; 
        c_foreach (i, clist_ix, list)
            if (++n % 10000 == 0) printf("%8d: %10zd\n", n, i.item->value);
        // Sort them...
        clist_ix_sort(&list); // mergesort O(n*log n)
        n = 0;
        puts("sorted");
        c_foreach (i, clist_ix, list)
            if (++n % 10000 == 0) printf("%8d: %10zd\n", n, i.item->value);
        clist_ix_destroy(&list);
    }
*/

#define declare_clist(...)    c_MACRO_OVERLOAD(declare_clist, __VA_ARGS__)

#define declare_clist_2(tag, Value) \
                               declare_clist_3(tag, Value, c_defaultDestroy)
#define declare_clist_3(tag, Value, valueDestroy) \
                               declare_clist_4(tag, Value, valueDestroy, c_defaultCompare)
#define declare_clist_4(tag, Value, valueDestroy, valueCompare) \
                               declare_clist_6(tag, Value, valueDestroy, Value, valueCompare, c_defaultGetRaw)
#define declare_clist_str() \
                               declare_clist_6(str, cstr_t, cstr_destroy, const char*, cstr_compareRaw, cstr_getRaw)

#define declare_clistTypes(tag, Value) \
    typedef struct clistnode_##tag { \
        struct clistnode_##tag *next; \
        Value value; \
    } clistnode_##tag; \
 \
    typedef struct clist_##tag { \
        clistnode_##tag *last; \
    } clist_##tag; \
 \
    typedef struct { \
        clistnode_##tag *item, **_last; \
    } clist_##tag##_iter_t

#define clist_init          {NULL}
#define clist_front(list)   (list).last->next->value
#define clist_back(list)    (list).last->value
#define clist_empty(list)   ((list).last == NULL)


#define declare_clist_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
 \
    declare_clistTypes(tag, Value); \
 \
    STC_INLINE clist_##tag \
    clist_##tag##_init(void) {clist_##tag x = clist_init; return x;} \
    STC_API void \
    clist_##tag##_destroy(clist_##tag* self); \
    STC_INLINE void \
    clist_##tag##_clear(clist_##tag* self) {clist_##tag##_destroy(self);} \
    STC_API void \
    clist_##tag##_pushBack(clist_##tag* self, Value value); \
    STC_API void \
    clist_##tag##_pushFront(clist_##tag* self, Value value); \
    STC_API void \
    clist_##tag##_pushN(clist_##tag *self, const Value in[], size_t size); \
    STC_API void \
    clist_##tag##_popFront(clist_##tag* self); \
    STC_API void \
    clist_##tag##_insertAfter(clist_##tag* self, clist_##tag##_iter_t pos, Value value); \
    STC_API void \
    clist_##tag##_eraseAfter(clist_##tag* self, clist_##tag##_iter_t pos); \
    STC_API void \
    clist_##tag##_spliceFront(clist_##tag* self, clist_##tag* other); \
    STC_API void \
    clist_##tag##_spliceAfter(clist_##tag* self, clist_##tag##_iter_t pos, clist_##tag* other); \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_findBefore(clist_##tag* self, RawValue val); \
    STC_API Value* \
    clist_##tag##_find(clist_##tag* self, RawValue val); \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_remove(clist_##tag* self, RawValue val); \
    STC_API void \
    clist_##tag##_sort(clist_##tag* self); \
 \
    STC_INLINE Value* \
    clist_##tag##_front(clist_##tag* self) {return &self->last->next->value;} \
    STC_INLINE Value* \
    clist_##tag##_back(clist_##tag* self) {return &self->last->value;} \
 \
    STC_INLINE clist_##tag##_iter_t \
    clist_##tag##_begin(clist_##tag* self) { \
        clistnode_##tag *head = self->last ? self->last->next : NULL; \
        clist_##tag##_iter_t it = {head, &self->last}; return it; \
    } \
    STC_INLINE clist_##tag##_iter_t \
    clist_##tag##_next(clist_##tag##_iter_t it) { \
        it.item = it.item == *it._last ? NULL : it.item->next; return it; \
    } \
    STC_INLINE clist_##tag##_iter_t \
    clist_##tag##_last(clist_##tag* self) { \
        clist_##tag##_iter_t it = {self->last, &self->last}; return it; \
    } \
 \
    implement_clist_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
    typedef RawValue clist_##tag##_rawvalue_t; \
    typedef Value clist_##tag##_value_t, clist_##tag##_input_t

    
/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_clist_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
  \
    STC_API void \
    clist_##tag##_destroy(clist_##tag* self) { \
        while (self->last) \
            clist_##tag##_popFront(self); \
    } \
 \
    STC_API void \
    clist_##tag##_pushBack(clist_##tag* self, Value value) { \
        _clist_insertAfter(self, tag, self->last, value); \
        self->last = entry; \
    } \
    STC_API void \
    clist_##tag##_pushFront(clist_##tag* self, Value value) { \
        _clist_insertAfter(self, tag, self->last, value); \
        if (!self->last) self->last = entry; \
    } \
    STC_API void \
    clist_##tag##_pushN(clist_##tag *self, const Value in[], size_t size) { \
        for (size_t i=0; i<size; ++i) clist_##tag##_pushBack(self, in[i]); \
    } \
    STC_API void \
    clist_##tag##_popFront(clist_##tag* self) { \
        _clist_eraseAfter(self, tag, self->last, valueDestroy); \
    } \
 \
    STC_API void \
    clist_##tag##_insertAfter(clist_##tag* self, clist_##tag##_iter_t pos, Value value) { \
        _clist_insertAfter(self, tag, pos.item, value); \
        if (!self->last || pos.item == self->last) self->last = entry; \
    } \
    STC_API void \
    clist_##tag##_eraseAfter(clist_##tag* self, clist_##tag##_iter_t pos) { \
        _clist_eraseAfter(self, tag, pos.item, valueDestroy); \
    } \
 \
    static inline void \
    _clist_##tag##_splice(clist_##tag* self, clist_##tag##_iter_t pos, clist_##tag* other, bool bottom) { \
        if (!pos.item) \
            self->last = pos.item = other->last; \
        else if (other->last) { \
            clistnode_##tag *next = pos.item->next; \
            pos.item->next = other->last->next; \
            other->last->next = next; \
            if (bottom && pos.item == self->last) self->last = other->last; \
        } \
        other->last = NULL; \
    } \
    STC_API void \
    clist_##tag##_spliceFront(clist_##tag* self, clist_##tag* other) { \
        _clist_##tag##_splice(self, clist_##tag##_last(self), other, false); \
    } \
    STC_API void \
    clist_##tag##_spliceAfter(clist_##tag* self, clist_##tag##_iter_t pos, clist_##tag* other) { \
        _clist_##tag##_splice(self, pos, other, true); \
    } \
 \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_findBefore(clist_##tag* self, RawValue val) { \
        clist_##tag##_iter_t prev = {self->last, &self->last}; \
        c_foreach (i, clist_##tag, *self) { \
            RawValue r = valueGetRaw(&i.item->value); \
            if (valueCompareRaw(&r, &val) == 0) { \
                return prev; \
            } \
            prev = i; \
        } \
        prev.item = NULL; return prev; \
    } \
 \
    STC_API Value* \
    clist_##tag##_find(clist_##tag* self, RawValue val) { \
        clist_##tag##_iter_t it = clist_##tag##_findBefore(self, val); \
        return it.item ? &it.item->next->value : NULL; \
    } \
 \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_remove(clist_##tag* self, RawValue val) { \
        clist_##tag##_iter_t it = clist_##tag##_findBefore(self, val); \
        if (it.item) clist_##tag##_eraseAfter(self, it); \
        return it; \
    } \
 \
    static inline int \
    clist_##tag##_sortCompare(const void* x, const void* y) { \
        RawValue a = valueGetRaw(&((clistnode_##tag *) x)->value); \
        RawValue b = valueGetRaw(&((clistnode_##tag *) y)->value); \
        return valueCompareRaw(&a, &b); \
    } \
    STC_API void \
    clist_##tag##_sort(clist_##tag* self) { \
        clistnode__base* last = _clist_mergesort((clistnode__base *) self->last->next, clist_##tag##_sortCompare); \
        self->last = (clistnode_##tag *) last; \
    }

#define _clist_insertAfter(self, tag, node, val) \
    clistnode_##tag *entry = c_new (clistnode_##tag), \
                    *next = self->last ? node->next : entry; \
    entry->value = val; \
    entry->next = next; \
    if (node) node->next = entry
    /* +: set self->last based on node */

#define _clist_eraseAfter(self, tag, node, valueDestroy) \
    clistnode_##tag* del = node->next, *next = del->next; \
    node->next = next; \
    if (del == next) self->last = NULL; \
    else if (self->last == del) self->last = node; \
    valueDestroy(&del->value); \
    free(del)

declare_clistTypes(_base, int);

/* Singly linked list Mergesort implementation by Simon Tatham. O(n*log n).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
static inline clistnode__base *
_clist_mergesort(clistnode__base *list, int (*cmp)(const void*, const void*)) {
    clistnode__base *p, *q, *e, *tail, *oldhead;
    int insize = 1, nmerges, psize, qsize, i;
    if (!list) return NULL;
    
    while (1) {
        p = list;
        oldhead = list;
        list = tail = NULL;
        nmerges = 0;

        while (p) {
            ++nmerges;
            q = p;
            psize = 0;
            for (i = 0; i < insize; ++i) {
                ++psize;
                q = (q->next == oldhead ? NULL : q->next);
                if (!q) break;
            }
            qsize = insize;

            while (psize > 0 || (qsize > 0 && q)) {
                if (psize == 0) {
                    e = q; q = q->next; --qsize;
                    if (q == oldhead) q = NULL;
                } else if (qsize == 0 || !q) {
                    e = p; p = p->next; --psize;
                    if (p == oldhead) p = NULL;
                } else if (cmp(p, q) <= 0) {
                    e = p; p = p->next; --psize;
                    if (p == oldhead) p = NULL;
                } else {
                    e = q; q = q->next; --qsize;
                    if (q == oldhead) q = NULL;
                }
                if (tail)
                    tail->next = e;
                else
                    list = e;
                tail = e;
            }
            p = q;
        }
        tail->next = list;

        if (nmerges <= 1)
            return tail;

        insize *= 2;
    }
}

#else
#define implement_clist_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw)
#endif

#endif
