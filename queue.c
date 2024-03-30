#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <linux/kernel.h>
#include <linux/string.h>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *lh = malloc(sizeof(struct list_head));

    /*if lh didn't malloc sucessfully*/
    if (!lh)
        return NULL;

    /*initialized the new list head*/
    INIT_LIST_HEAD(lh);

    return lh;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    if (list_empty(l) == true) {
        free(l);
        return;
    }

    element_t *now, *safe;
    list_for_each_entry_safe (now, safe, l, list) {
        q_release_element(now);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    /*malloc space for new entry item*/
    element_t *le = malloc(sizeof(element_t));

    if (!le)
        return false;

    le->value = malloc(sizeof(char) * strlen(s) + 1);
    if (!le->value) {
        free(le);
        return false;
    }
    le->value = strncpy(le->value, s, strlen(s) + 1);
    list_add(&le->list, head);


    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    /*malloc space for new entry item*/
    element_t *le = malloc(sizeof(element_t));

    if (!le)
        return false;


    le->value = malloc(sizeof(char) * (strlen(s) + 1));
    if (!le->value) {
        free(le);
        return false;
    }
    le->value = strncpy(le->value, s, strlen(s) + 1);

    list_add_tail(&le->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *temp = list_entry(head->next, element_t, list);

    if (!temp) {
        return NULL;
    }
    if (sp) {
        strncpy(sp, list_entry(head->next, element_t, list)->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(head->next);
    return temp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *temp = list_entry(head->prev, element_t, list);

    if (!temp) {
        return NULL;
    }

    if (sp) {
        strncpy(sp, list_entry(head->prev, element_t, list)->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(head->prev);

    return temp;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    /*in list.h line 378 */
    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    struct list_head *slow = head->next;
    struct list_head *fast = head->next;

    while (fast->next != head && fast->next->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    list_del_init(slow);

    q_release_element(list_entry(slow, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;

    element_t *now, *safe, *last;
    last = NULL;
    bool checkDel = false;
    list_for_each_entry_safe (now, safe, head, list) {
        if (!last) {
            last = now;
            continue;
        }

        if (strcmp(now->value, last->value) == 0) {
            list_del(&last->list);
            q_release_element(last);
            checkDel = true;
        } else {
            if (checkDel == true) {
                list_del(&last->list);
                q_release_element(last);
            }
            checkDel = false;
        }
        last = now;
    }

    if (checkDel == true) {
        list_del(&last->list);
        q_release_element(last);
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    /*the list_head l1 is the first item need to be swap,l2 is the second*/
    struct list_head *l1 = head->next;
    struct list_head *l2 = head->next->next;

    while (l1 != head && l2 != head) {
        list_move(l1, l2);

        l2 = l1->next->next;
        l1 = l1->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *now;

    struct list_head *temp = head->prev;
    head->prev = head->next;
    head->next = temp;

    list_for_each (now, head) {
        /*swap*/
        temp = now->prev;
        now->prev = now->next;
        now->next = temp;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    struct list_head *now;
    struct list_head *start = head->next;
    struct list_head *safe;
    int count = 1;

    list_for_each_safe (now, safe, head) {
        if (count == k) {
            struct list_head *hd = start;
            struct list_head *tail = now;
            for (int i = 0; i < k; i++) {
                struct list_head *tmp = start->next;
                start->next = start->prev;
                start->prev = tmp;
                start = start->prev;
            }
            tail->prev = hd->next;
            hd->next = start;
            tail->prev->next = tail;
            hd->next->prev = hd;



            count = 0;
        }
        count++;
    }
}


struct list_head *merge(struct list_head *l1, struct list_head *l2, bool des)
{
    struct list_head *temp;
    struct list_head *head;

    if (l1 && l2) {
        if (des == false) {
            if (strcmp(list_entry(l1, element_t, list)->value,
                       list_entry(l2, element_t, list)->value) < 0) {
                temp = l1;
                l1 = l1->next;
            } else {
                temp = l2;
                l2 = l2->next;
            }
        } else {
            if (strcmp(list_entry(l1, element_t, list)->value,
                       list_entry(l2, element_t, list)->value) > 0) {
                temp = l1;
                l1 = l1->next;
            } else {
                temp = l2;
                l2 = l2->next;
            }
        }
    } else if (l1) {
        temp = l1;
        l1 = l1->next;
    } else {
        temp = l2;
        l2 = l2->next;
    }
    head = temp;

    while (l1 && l2) {
        if (des == false) {
            if (strcmp(list_entry(l1, element_t, list)->value,
                       list_entry(l2, element_t, list)->value) < 0) {
                temp->next = l1;
                temp = temp->next;
                l1 = l1->next;
            } else {
                temp->next = l2;
                temp = temp->next;
                l2 = l2->next;
            }
        } else {
            if (strcmp(list_entry(l1, element_t, list)->value,
                       list_entry(l2, element_t, list)->value) > 0) {
                temp->next = l1;
                temp = temp->next;
                l1 = l1->next;
            } else {
                temp->next = l2;
                temp = temp->next;
                l2 = l2->next;
            }
        }
    }
    if (l1)
        temp->next = l1;
    if (l2)
        temp->next = l2;

    return head;
}

struct list_head *mergeSort(struct list_head *head, bool des)
{
    if (!head || head->next == NULL)
        return head;

    struct list_head *fast = head->next;
    struct list_head *slow = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    struct list_head *l1 = mergeSort(head, des);
    struct list_head *l2 = mergeSort(fast, des);

    return merge(l1, l2, des);
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    head->prev->next = NULL;
    struct list_head *temp, *last;
    temp = mergeSort(head->next, descend);
    last = head;
    while (temp != NULL) {
        last->next = temp;
        temp->prev = last;
        last = last->next;
        temp = temp->next;
    }
    head->prev = last;
    last->next = head;
}



/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    struct list_head *tail = head->prev;
    struct list_head *now = tail;
    element_t *min = list_entry(tail, element_t, list);

    while (now != head) {
        if (strcmp(list_entry(now, element_t, list)->value, min->value) > 0) {
            struct list_head *tmp;
            tmp = now->prev;
            list_del(now);
            q_release_element(list_entry(now, element_t, list));
            now = tmp;
        } else {
            min = list_entry(now, element_t, list);
            now = now->prev;
        }
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    struct list_head *tail = head->prev;
    struct list_head *now = tail;
    element_t *max = list_entry(tail, element_t, list);

    while (now != head) {
        if (strcmp(list_entry(now, element_t, list)->value, max->value) < 0) {
            struct list_head *tmp;
            tmp = now->prev;
            list_del(now);
            q_release_element(list_entry(now, element_t, list));
            now = tmp;
        } else {
            max = list_entry(now, element_t, list);
            now = now->prev;
        }
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    struct list_head *h1 = list_entry(head->next, queue_contex_t, chain)->q;
    struct list_head *now, *safe;

    list_for_each_safe (now, safe, head) {
        if (list_entry(now, queue_contex_t, chain)->q != h1) {
            list_splice_init(list_entry(now, queue_contex_t, chain)->q, h1);
        }
    }

    q_sort(h1, descend);
    return 1;
}



static struct list_head *list_merge(struct list_head *a,
                                    struct list_head *b,
                                    bool descend)
{
    struct list_head *head = NULL, **tail = &head;
    if (descend == false) {
        for (;;) {
            /* if equal, take 'a' -- important for sort stability */
            if (strcmp(list_entry(a, element_t, list)->value,
                       list_entry(b, element_t, list)->value) <= 0) {
                *tail = a;
                tail = &a->next;
                a = a->next;
                if (!a) {
                    *tail = b;
                    break;
                }
            } else {
                *tail = b;
                tail = &b->next;
                b = b->next;
                if (!b) {
                    *tail = a;
                    break;
                }
            }
        }
    } else {
        for (;;) {
            /* if equal, take 'a' -- important for sort stability */
            if (strcmp(list_entry(a, element_t, list)->value,
                       list_entry(b, element_t, list)->value) >= 0) {
                *tail = a;
                tail = &a->next;
                a = a->next;
                if (!a) {
                    *tail = b;
                    break;
                }
            } else {
                *tail = b;
                tail = &b->next;
                b = b->next;
                if (!b) {
                    *tail = a;
                    break;
                }
            }
        }
    }

    return head;
}

/*
 * Combine final list merge with restoration of standard doubly-linked
 * list structure.  This approach duplicates code from merge(), but
 * runs faster than the tidier alternatives of either a separate final
 * prev-link restoration pass, or maintaining the prev links
 * throughout.
 */

static void merge_final(struct list_head *head,
                        struct list_head *a,
                        struct list_head *b,
                        bool descend)
{
    struct list_head *tail = head;

    if (descend == false) {
        for (;;) {
            /* if equal, take 'a' -- important for sort stability */
            if (strcmp(list_entry(a, element_t, list)->value,
                       list_entry(b, element_t, list)->value) <= 0) {
                tail->next = a;
                a->prev = tail;
                tail = a;
                a = a->next;
                if (!a)
                    break;
            } else {
                tail->next = b;
                b->prev = tail;
                tail = b;
                b = b->next;
                if (!b) {
                    b = a;
                    break;
                }
            }
        }
    } else {
        for (;;) {
            /* if equal, take 'a' -- important for sort stability */
            if (strcmp(list_entry(a, element_t, list)->value,
                       list_entry(b, element_t, list)->value) >= 0) {
                tail->next = a;
                a->prev = tail;
                tail = a;
                a = a->next;
                if (!a)
                    break;
            } else {
                tail->next = b;
                b->prev = tail;
                tail = b;
                b = b->next;
                if (!b) {
                    b = a;
                    break;
                }
            }
        }
    }


    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        /*
         * If the merge is highly unbalanced (e.g. the input is
         * already sorted), this loop may run many iterations.
         * Continue callbacks to the client even though no
         * element comparison is needed, so the client's cmp()
         * routine can invoke cond_resched() periodically.
         */
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

void list_sort(struct list_head *head, bool descend)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    /*
     * Data structure invariants:
     * - All lists are singly linked and null-terminated; prev
     *   pointers are not maintained.
     * - pending is a prev-linked "list of lists" of sorted
     *   sublists awaiting further merging.
     * - Each of the sorted sublists is power-of-two in size.
     * - Sublists are sorted by size and age, smallest & newest at front.
     * - There are zero to two sublists of each size.
     * - A pair of pending sublists are merged as soon as the number
     *   of following pending elements equals their size (i.e.
     *   each time count reaches an odd multiple of that size).
     *   That ensures each later final merge will be at worst 2:1.
     * - Each round consists of:
     *   - Merging the two sublists selected by the highest bit
     *     which flips when count is incremented, and
     *   - Adding an element from the input as a size-1 sublist.
     */
    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (likely(bits)) {
            struct list_head *a = *tail, *b = a->prev;

            a = list_merge(b, a, descend);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = list_merge(pending, list, descend);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(head, pending, list, descend);
}



static inline size_t run_size(struct list_head *head)
{
    if (!head)
        return 0;
    if (!head->next)
        return 1;
    return (size_t) (head->next->prev);
}

struct pair {
    struct list_head *head, *next;
};

static size_t stk_size;

static struct list_head *tim_merge(struct list_head *a,
                                   struct list_head *b,
                                   int des)
{
    struct list_head *head = NULL;
    struct list_head **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (des * strcmp(list_entry(a, element_t, list)->value,
                         list_entry(b, element_t, list)->value) <=
            0) {
            *tail = a;
            tail = &(a->next);
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &(b->next);
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void build_prev_link(struct list_head *head,
                            struct list_head *tail,
                            struct list_head *list)
{
    tail->next = list;
    do {
        list->prev = tail;
        tail = list;
        list = list->next;
    } while (list);

    /* The final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

static void tim_merge_final(struct list_head *head,
                            struct list_head *a,
                            struct list_head *b,
                            int des)
{
    struct list_head *tail = head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (des * strcmp(list_entry(a, element_t, list)->value,
                         list_entry(b, element_t, list)->value) <=
            0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    build_prev_link(head, tail, b);
}

static struct pair find_run(struct list_head *list, int des)
{
    size_t len = 1;
    struct list_head *next = list->next, *head = list;
    struct pair result;

    if (!next) {
        result.head = head, result.next = next;
        return result;
    }

    if (des * strcmp(list_entry(list, element_t, list)->value,
                     list_entry(next, element_t, list)->value) >
        0) {
        /* decending run, also reverse the list */
        struct list_head *prev = NULL;
        do {
            len++;
            list->next = prev;
            prev = list;
            list = next;
            next = list->next;
            head = list;
        } while (next &&
                 des * strcmp(list_entry(list, element_t, list)->value,
                              list_entry(next, element_t, list)->value) >
                     0);
        list->next = prev;
    } else {
        do {
            len++;
            list = next;
            next = list->next;
        } while (next && strcmp(list_entry(list, element_t, list)->value,
                                list_entry(next, element_t, list)->value) > 0);
        list->next = NULL;
    }
    head->prev = NULL;
    head->next->prev = (struct list_head *) len;
    result.head = head, result.next = next;
    return result;
}

static struct list_head *merge_at(struct list_head *at, int des)
{
    size_t len = run_size(at) + run_size(at->prev);
    struct list_head *prev = at->prev->prev;
    struct list_head *list = tim_merge(at->prev, at, des);
    list->prev = prev;
    list->next->prev = (struct list_head *) len;
    --stk_size;
    return list;
}

static struct list_head *merge_force_collapse(struct list_head *tp, int des)
{
    while (stk_size >= 3) {
        if (run_size(tp->prev->prev) < run_size(tp)) {
            tp->prev = merge_at(tp->prev, des);
        } else {
            tp = merge_at(tp, des);
        }
    }
    return tp;
}

static struct list_head *merge_collapse(struct list_head *tp, int des)
{
    int n;
    while ((n = stk_size) >= 2) {
        if ((n >= 3 &&
             run_size(tp->prev->prev) <= run_size(tp->prev) + run_size(tp)) ||
            (n >= 4 && run_size(tp->prev->prev->prev) <=
                           run_size(tp->prev->prev) + run_size(tp->prev))) {
            if (run_size(tp->prev->prev) < run_size(tp)) {
                tp->prev = merge_at(tp->prev, des);
            } else {
                tp = merge_at(tp, des);
            }
        } else if (run_size(tp->prev) <= run_size(tp)) {
            tp = merge_at(tp, des);
        } else {
            break;
        }
    }

    return tp;
}

void timsort(struct list_head *head, bool descend)
{
    stk_size = 0;
    int des;
    if (descend == false) {
        des = 1;
    } else {
        des = -1;
    }

    struct list_head *list = head->next, *tp = NULL;
    if (head == head->prev)
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    do {
        /* Find next run */
        struct pair result = find_run(list, des);
        result.head->prev = tp;
        tp = result.head;
        list = result.next;
        stk_size++;
        tp = merge_collapse(tp, des);
    } while (list);

    /* End of input; merge together all the runs. */
    tp = merge_force_collapse(tp, des);

    /* The final merge; rebuild prev links */
    struct list_head *stk0 = tp, *stk1 = stk0->prev;
    while (stk1 && stk1->prev)
        stk0 = stk0->prev, stk1 = stk1->prev;
    if (stk_size <= 1) {
        build_prev_link(head, head, stk0);
        return;
    }
    tim_merge_final(head, stk1, stk0, des);
}