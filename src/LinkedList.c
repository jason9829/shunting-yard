#include "LinkedList.h"
#include <stdio.h>
#include <malloc.h>
#include "Exception.h"
#include "Error.h"


int *LinkedListAddToHead_wNewLinkedListAddress(LinkedList *List, void *data){

  if(*(int*)(data) == NULL){
    throwSimpleError(ERR_INVALID_DATA,"Data trying to add is NULL");
  }
  else{
    ListItem *newLinkedList = (struct ListItem*)malloc (sizeof(struct ListItem));
    if(List->head == NULL){
      newLinkedList->data = *(int*)(data);
      newLinkedList->next = NULL;
      List->count++;
      List->head = newLinkedList;
      List->tail = newLinkedList;
    }
    else{
      newLinkedList->data = *(int*)(data);
      newLinkedList->next = List->head;     // Take previous node as next
      if(List->count == 1){     // if 2 block (head->newLinkedList) take previous as tail else tail remain the same
        List->tail = newLinkedList->next;              // previous node as tail
      }
      List->count++;
      List->head = newLinkedList;
    }
    return newLinkedList;
  }
}

void LinkedListAddToHead(LinkedList *List,void *data){
  if(*(int*)(data) == NULL){
    throwSimpleError(ERR_INVALID_DATA,"Data trying to add is NULL");
  }
  else{
    ListItem *newLinkedList = (struct ListItem*)malloc (sizeof(struct ListItem));
    if(List->head == NULL){
      newLinkedList->data = *(int*)(data);
      newLinkedList->next = NULL;
      List->count++;
      List->head = newLinkedList;
      List->tail = newLinkedList;
    }
    else{
      newLinkedList->data = *(int*)(data);
      newLinkedList->next = List->head;     // Take previous node as next
      if(List->count == 1){     // if 2 block (head->newLinkedList) take previous as tail else tail remain the same
        List->tail = newLinkedList->next;              // previous node as tail
      }
      List->count++;
      List->head = newLinkedList;
    }
  }
}
ListItem *LinkedListRemoveFromHead(LinkedList *List){
  if(List->head == NULL){
    throwSimpleError(ERR_LINKEDLIST_NULL,"Empty linkedlist detected");
  }
  else{
    ListItem *removedLinkedList = (struct ListItem*)malloc (sizeof(struct ListItem));
    removedLinkedList = List->head;
    int *nextItemAddress;
    nextItemAddress = List->head->next;

    /*  List
     *      BEFORE
     *                   (remove this)          (nextItemAddress)
     *                       +-----+      next      +-----+   next
     *           head----->  |  1  |--------------> |  2  |---------> NULL
     *                       +-----+                +-----+
     *                     removedLinkedList
     *       AFTER
     *                       +-----+      next
     *           head----->  |  2  |--------------> NULL
     *                       +-----+
     */

    List->head->next = NULL;
    List->head = nextItemAddress;
    List->count--;
    return removedLinkedList;
  }
}
