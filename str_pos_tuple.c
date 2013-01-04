#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef void (*STR_FUNC)(void *data, char *s);

struct callback {
    STR_FUNC f;
    void *data;
};
#define CALL(pcb, s) ((pcb->f)(pcb->data, s))

struct pos_str_tuple {
    unsigned int pos;
    char *s;
};

struct tuple_array {
    unsigned int cnt;
    unsigned int size;
    unsigned int pos;
    struct pos_str_tuple *array;
};

void tuple_array_init(struct tuple_array *pta) {
    pta->pos = 0;
    pta->cnt = 0;
    pta->size = 10;
    pta->array = malloc(10 * sizeof(struct pos_str_tuple));
}

void tuple_array_release(struct tuple_array *pta) {
    unsigned int i;

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
    struct tuple_array *pta,
    int doc_id
) {
    unsigned int i;
    unsigned int cnt = pta->cnt;
    unsigned int width = sizeof(struct pos_str_tuple);

    if (!cnt) return;

    struct pos_str_tuple ppst[cnt];

    memcpy(ppst, pta->array, cnt * width);

    qsort(
        ppst,
        cnt,
        width,
        tuple_str_compare
    );

    char *curr_s = ppst[0].s;
    printf("%s %d", curr_s, doc_id);

    for (i = 0; i < cnt; ++i) {
        if (strcmp(curr_s, ppst[i].s)) {
            curr_s = ppst[i].s;
            printf("\n%s %d", curr_s, doc_id);
        }
        printf(" %d", ppst[i].pos);
    }
    printf("\n");
}

void test_add_strings(struct callback *pcb) {
    unsigned int i = 0;

    CALL(pcb, "foo");
    for (i = 0; i < 40; ++i) {
        CALL(pcb, "bar");
    }
    CALL(pcb, "yo");
    CALL(pcb, "foo");
}

void test(doc_id) {
    struct tuple_array ta;

    tuple_array_init(&ta);
    struct callback cb;
    cb.f = (STR_FUNC)tuple_array_add;
    cb.data = &ta;

    test_add_strings(&cb);
    printf("\n\n");

    dump_str_positions(&ta, doc_id);
    tuple_array_release(&ta);
}

int main(int argc, char **argv) {
    unsigned int i;

    for (i = 0; i < 1000; ++i) {
        test(i);
    }
}
