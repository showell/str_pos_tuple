#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct pos_str_tuple {
    unsigned int pos;
    char *s;
};

struct tuple_array {
    int cnt;
    int size;
    int pos;
    struct pos_str_tuple *array;
};

void tuple_array_init(struct tuple_array *pta) {
    pta->pos = 0;
    pta->cnt = 0;
    pta->size = 10;
    pta->array = malloc(10 * sizeof(struct pos_str_tuple));
}

void tuple_array_release(struct tuple_array *pta) {
    int i;

    for (i = 0; i < pta->cnt; ++i) {
        free(pta->array[i].s);
    }
    free(pta->array);
}

void tuple_array_append(struct tuple_array *pta, char *s) {
    if (pta->cnt >= pta->size) {
        pta->size *= 2;
        pta->array = realloc(
            pta->array,
            pta->size * sizeof(struct pos_str_tuple)
        );
    }
    pta->array[pta->cnt].pos = pta->pos;
    pta->array[pta->cnt].s = strdup(s);
    ++pta->cnt;
}

void tuple_array_add(struct tuple_array *pta, char *s) {
    tuple_array_append(pta, s);
    if (!strcmp(s, "foo")) {
        tuple_array_append(pta, "foo2");
    }
    ++pta->pos;
}

int tuple_str_compare(const void *v1, const void *v2) {
    struct pos_str_tuple *p1 = (struct pos_str_tuple *)v1;
    struct pos_str_tuple *p2 = (struct pos_str_tuple *)v2;

    int result = strcmp(p1->s, p2->s);
    if (result != 0) return result;

    return p1->pos - p2->pos;
}

void dump_str_positions(
    struct tuple_array *pta
) {
    int i;
    int cnt = pta->cnt;
    int width = sizeof(struct pos_str_tuple);
    struct pos_str_tuple *ppst;

    if (!cnt) return;

    ppst = malloc(cnt * width);
    memcpy(ppst, pta->array, cnt * width);

    qsort(
        ppst,
        cnt,
        width,
        tuple_str_compare
    );

    char *curr_s = ppst[0].s;
    printf("%s", curr_s);

    for (i = 0; i < cnt; ++i) {
        if (strcmp(curr_s, ppst[i].s)) {
            curr_s = ppst[i].s;
            printf("\n%s", curr_s);
        }
        printf(" %d", ppst[i].pos);
    }
    printf("\n");

    free(ppst);
}


void test() {
    struct tuple_array sa;
    int i = 0;

    tuple_array_init(&sa);
    tuple_array_add(&sa, "foo");
    for (i = 0; i < 40; ++i) {
        tuple_array_add(&sa, "bar");
    }
    tuple_array_add(&sa, "yo");
    tuple_array_add(&sa, "foo");

    dump_str_positions(&sa);
    tuple_array_release(&sa);
}

int main(int argc, char **argv) {
    int i;

    for (i = 0; i < 1000; ++i) {
        test();
    }
}
