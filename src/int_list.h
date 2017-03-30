/* Thibaut Ehlinger
 * Stan Wilhelm
 */

#ifndef __INT_LIST_H__
#define __INT_LIST_H__

#include <stdlib.h>

typedef struct intlist {
    struct intlist* next;
    int value;
} intlist;

intlist* new_intlist(int val);

void delete_intlist(intlist* list);

intlist* add_intlist(intlist *intl1, intlist *intl2);

int size_intlist(intlist* intl);

int get_intlist_element(intlist* intl, int elemnbr);

#endif

