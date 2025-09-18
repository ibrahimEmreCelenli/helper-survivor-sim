
 
#include "list.h"


List *create_list(size_t datasize, int capacity) {
    List *list = malloc(sizeof(List));
    memset(list, 0, sizeof(List));

    list->datasize = datasize;
    list->nodesize = sizeof(Node) + datasize;

    list->startaddress = malloc(list->nodesize * capacity);
    list->endaddress = list->startaddress + (list->nodesize * capacity);
    memset(list->startaddress, 0, list->nodesize * capacity);

    list->lastprocessed = (Node *)list->startaddress;

    list->number_of_elements = 0;
    list->capacity = capacity;


    list->self = list;
    list->add = add;
    list->removedata = removedata;
    list->removenode = removenode;
    list->pop = pop;
    list->peek = peek;
    list->destroy = destroy;
    list->printlist = printlist;
    list->printlistfromtail = printlistfromtail;
    return list;
}

static Node *find_memcell_fornode(List *list) {
    Node *node = NULL;

    Node *temp = list->lastprocessed;
    while ((char *)temp < list->endaddress) {
        if (temp->occupied == 0) {
            node = temp;
            break;
        } else {
            temp = (Node *)((char *)temp + list->nodesize);
        }
    }
    if (node == NULL) {

        temp = (Node *)list->startaddress;
        while (temp < list->lastprocessed) {
            if (temp->occupied == 0) {
                node = temp;
                break;
            } else {
                temp = (Node *)((char *)temp + list->nodesize);
            }
        }
    }
    return node;
}


Node *add(List *list, void *data) {
    Node *node = NULL;


    if (list->number_of_elements >= list->capacity) {
        perror("list is full!");
        return NULL;
    }

    /*first find an unoccupied memcell and insert into it*/
    node = find_memcell_fornode(list);

    if (node != NULL) {
        /*create_node*/
        node->occupied = 1;
        memcpy(node->data, data, list->datasize);

        /*change new node into head*/
        if (list->head != NULL) {
            Node *oldhead = list->head;
            oldhead->prev = node;
            node->prev = NULL;
            node->next = oldhead;
        }

        list->head = node;
        list->lastprocessed = node;
        list->number_of_elements += 1;
        if (list->tail == NULL) {
            list->tail = list->head;
        }
    } else {
        perror("list is full!");
    }

    return node;
}

int removedata(List *list, void *data) {
    Node *temp = list->head;
    while (temp->next != NULL && memcmp(temp->data, data, list->datasize) != 0) {
        temp = temp->next;
    }
    if (temp != NULL) {
        Node *prevnode = temp->prev;
        Node *nextnode = temp->next;
        if (prevnode != NULL) {
            prevnode->next = nextnode;
        }
        if (nextnode != NULL) {
            nextnode->prev = prevnode;
        }

        temp->next = NULL;
        temp->prev = NULL;
        temp->occupied = 0;
        if(temp == list->head)
            list->head=nextnode;

        list->number_of_elements--;
        return 0;
    }
    return 1;
}

void *pop(List *list, void *dest) {
    if (list->head != NULL) {
        Node *node = list->head;
        if (removenode(list, node) == 0) {
            memcpy(dest, node->data, list->datasize);
            return dest;
        }
    }
    dest = NULL;
    return NULL;
}

void *peek(List *list) {
    if (list->head != NULL) return list->head->data;

    return NULL;
}


int removenode(List *list, Node *node) {
    
    
    if (node != NULL) {
        Node *prevnode = node->prev;
        Node *nextnode = node->next;
        if (prevnode != NULL) {
            prevnode->next = nextnode;
        }
        if (nextnode != NULL) {
            nextnode->prev = prevnode;
        }
        node->next = NULL;
        node->prev = NULL;

        node->occupied = 0;


        list->number_of_elements--;


        if (node == list->tail) {
            list->tail = prevnode;
        }

        if (node == list->head) {
            list->head = nextnode;
        }
        list->lastprocessed = node;
        return 0;
    }

    return 1;
}


void destroy(List *list) {
    free(list->startaddress);
    memset(list, 0, sizeof(List));
    free(list);
}


void printlist(List *list, void (*print)(void *)) {
    Node *temp = list->head;
    while (temp != NULL) {
        print(temp->data);
        temp = temp->next;
    }
}

void printlistfromtail(List *list, void (*print)(void *)) {
    Node *temp = list->tail;
    while (temp != NULL) {
        print(temp->data);
        temp = temp->prev;
    }
}

void * nth_element(List* list,int* n){
    int i=0;
    Node* cursor;
    cursor = list->head;
    while(i!=*n&& cursor->next != NULL){
    cursor=cursor->next;
    
    i++;    
    }
    void * tbr=malloc(list->datasize);
    memcpy(tbr,cursor->data,list->datasize);
    return cursor->data;
}
