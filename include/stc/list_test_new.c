#include "cstr.h"
#include "forward.h"

forward_clist(i32, int);
forward_clist(pnt, struct Point);

struct MyStruct {
    clist_i32 intlst;
    clist_pnt pntlst;
} typedef MyStruct;


#define f_TAG i32
#define i_VAL int
#include "clist.h"

struct Point { int x, y; } typedef Point;
int point_compare(const Point* a, const Point* b) {
    int c = c_default_compare(&a->x, &b->x);
    return c ? c : c_default_compare(&a->y, &b->y);
}
#define f_TAG pnt
#define i_VAL Point
#define i_CMP point_compare
#include "clist.h"

#define i_VAL float
#include "clist.h"

#define i_VAL_str
#include "clist.h"


int main()
{
    clist_i32 lst = clist_i32_init();
    clist_i32_push_back(&lst, 123);
    clist_i32_del(&lst);

    clist_float flst = clist_float_init();
    clist_float_push_back(&flst, 123.3);
    clist_float_del(&flst);

    clist_pnt plst = clist_pnt_init();
    clist_pnt_push_back(&plst, (Point){42, 14});
    clist_pnt_push_back(&plst, (Point){32, 94});
    clist_pnt_push_back(&plst, (Point){62, 81});
    clist_pnt_sort(&plst);
    c_foreach (i, clist_pnt, plst)
        printf(" (%d %d)", i.ref->x, i.ref->y);
    puts("");
    clist_pnt_del(&plst);

    clist_str slst = clist_str_init();
    clist_str_emplace_back(&slst, "Hello, friend");
    clist_str_del(&slst);
}