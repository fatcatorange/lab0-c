#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

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
    if (sp)
        strncpy(sp, list_entry(head->next, element_t, list)->value, bufsize);

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

    if (sp)
        strncpy(sp, list_entry(head->prev, element_t, list)->value, bufsize);

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
    if (!head || list_empty(head))
        return false;

    int size = q_size(head);
    size /= 2;

    struct list_head *now = head;
    for (int i = 0; i <= size; i++)
        now = now->next;

    list_del_init(now);

    q_release_element(list_entry(now, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    /*the list_head l1 is the first item need to be swap,l2 is the second*/
    struct list_head *l1 = head->next;
    struct list_head *l2 = head->next->next;

    while (l1 != head && l2 != head) {
        l1->prev->next = l2;
        l2->next->prev = l1;
        l2->prev = l1->prev;
        l1->prev = l2;
        l1->next = l2->next;
        l2->next = l1;

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
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
