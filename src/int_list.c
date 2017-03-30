/* Thibaut Ehlinger
 * Stan Wilhelm
 */

#include "int_list.h"

intlist* new_intlist(int val)
	{
    intlist *new_one = malloc(sizeof(*new_one));
    new_one->next = NULL;
    new_one->value = val;
    return new_one;
}

void delete_intlist(intlist* list)
	{
    intlist* temp = list;
    while(list != NULL)
	{
        temp = list->next;
        free(list);
        list = temp;
    }
}

intlist* add_intlist(intlist *intl1, intlist *intl2)
	{
    if(intl1 == NULL)
	{
        return intl2;
    }
    else{
        intlist *temp = intl1;
        while(intl1->next != NULL)
            intl1 = intl1->next;
        intl1->next = intl2;
        return temp;
    }
}

int size_intlist(intlist* intl)
	{
    int size = 0;
    while(intl != NULL)
	{
        size++;
        intl = intl->next;
    }
    return size;
}

int get_intlist_element(intlist* intl, int elemnbr)
	{
	int i;
    for(i=0; i < elemnbr; i++)
        intl = intl->next;
    return intl->value;
}

