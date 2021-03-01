//NAME: Conner Yang
//EMAIL: conneryang@g.ucla.edu
//ID: 905417287

//
//  SortedList.c
//  2A
//
//  Created by Conner Yang on 2/12/21.
//  Copyright © 2021 Conner Yang. All rights reserved.
//

#include "SortedList.h"
#include <string.h>
#include <sched.h>

/**
* SortedList_insert ... insert an element into a sorted list
*
*    The specified element will be inserted in to
*    the specified list, which will be kept sorted
*    in ascending order based on associated keys
*
* @param SortedList_t *list ... header for the list
* @param SortedListElement_t *element ... element to be added to the list
*/
void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
        // If there is no element to insert, simply return
    if (element == NULL)
    {
        return;
    }
    
        // Critical section upcoming when looking at the list (it could be changed by another thread)
    if (opt_yield & INSERT_YIELD)
    {
        sched_yield();
    }
    
        // If the list is empty, make element the head element
    if (list == NULL)
    {
        list = element;
        list->next = element;
        list->prev = element;
        return;     // No need to continue, inserted the one element
    }
    
    SortedList_t* current = list;
    while (current->next != list && strcmp(element->key, current->next->key) > 0)
    {
        current = current->next;
    }
    
        // Update prev and next pointers
    element->prev = current;
    element->next = current->next;
    current->next->prev = element;
    current->next = element;
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *    The specified element will be removed from whatever
 *    list it is currently in.
 *
 *    Before doing the deletion, we check to make sure that
 *    next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element)
{
        // Check for corrupted prev/next pointers
    if (element == NULL || element->next->prev != element || element->prev->next != element)
    {
        return 1;
    }
    
        // Critical section due to modifying the linked list
    if (opt_yield & DELETE_YIELD)
        sched_yield();
    
        // Update prev/next pointers to reflect deleted element
    element->next->prev = element->prev;
    element->prev->next = element->next;
    
    return 0;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *    The specified list will be searched for an
 *    element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
        // Check that there are any elements in the list
    if (list == NULL)
    {
        return NULL;
    }
    
    SortedList_t* current = list->next;     // Start at first element in the list
        // Critical section: Another thread could insert an element that we pass
    if (opt_yield & LOOKUP_YIELD)
        sched_yield();
    
    do
    {
        if (strcmp(key, current->key) == 0)
            return current;
            // If the below happens, we missed it
        if (strcmp(key, current->key) < 0)
            break;
        current = current->next;    // Update current pointer to next item
    }
    while (current != list);
    
        // If we did not return the current pointer at some point, it was not found.
    return NULL;
}

/**
 * SortedList_length ... count elements in a sorted list
 *    While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *       -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list)
{
    if (list == NULL)
        return -1;
    
        // Start at head pointer, 0th element
    SortedListElement_t* current = list;
    int len = 0;
    
        // Critical section: Another thread could insert a node as soon as we go to the first node
    if (opt_yield & LOOKUP_YIELD)
        sched_yield();
    
        // Iterate through each element and increment length variable "len"
        // current or current->next for condition?
    while (current != list)
    {
            // Checking for list corruption (pointers)
        if (current->next == NULL || current->prev->next != current || current->next->prev != current)
        {
            return -1;
        }
        
        len++;
        current = current->next;
    }
    
    return len;
}
