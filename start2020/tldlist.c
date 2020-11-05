#include "tldlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//
// Created by James Ross on 29/10/2020.
//
//Initialise Structs
struct tldlist {

    struct date *begin_date;
    struct date  *end_date;
    long add_success;
    struct tldnode *root;
    long TLDSize;

};

struct tldnode {

    char TLDstr[3];
    long TLDcount;
    struct tldnode *parent;
    struct tldnode *left;
    struct tldnode *right;
    int balance;
    int height;
    bool found;

};

struct tlditerator {

    struct tldnode *root;

};


//Creates an instance of tld list
TLDList *tldlist_create( Date *begin,  Date *end){

    struct tldlist *p;

    //Gets new address memory for list if possible, if yes initialize list
    if ((p = (struct tldlist *)malloc(sizeof(struct tldlist))) != NULL){

        p->begin_date = begin;
        p->end_date = end;
        p->root = NULL;
        p->TLDSize = 0;
        p->add_success = 0;

    }

    return p;

}

//Gets the height of the current node, used for avl
int get_height(TLDNode *n){

    if(n == NULL){

        return -1;

    }

    return n->height;

}

//Updates a nodes height when rebalancing for avl
void reheight(TLDNode *n){

    if(n != NULL){

        if(get_height(n->right) > get_height(n->left)){

            n->height = 1 + get_height(n->right);

        }else{

            n->height = 1 + get_height(n->left);

        }


    }

}

//Sets a nodes balance value to be used for balancing avl
void setBalance(TLDNode *n){

    reheight(n);

    n->balance = get_height(n->right) - get_height(n->left);

}

//Rotates node to the left to rebalance the tree
TLDNode* rotateLeft(TLDNode *a){

    struct tldnode *b = a->right;
    b->parent = a->parent;

    a->right = b->left;

    if(a->right != NULL){

        a->right->parent = a;

    }

    b->left = a;
    a->parent = b;

    if(b->parent != NULL){

        if(b->parent->right == a){

            b->parent->right = b;

        } else {

            b->parent->left = b;

        }
    }

    setBalance(a);
    setBalance(b);

    return b;

}

//Rotates node to the right to rebalance the tree
TLDNode* rotateRight(TLDNode *a){

    struct tldnode *b = a->left;
    b->parent = a->parent;

    a->left = b->right;

    if(a->left != NULL){

        a->left->parent = a;

    }

    b->right = a;
    a->parent = b;

    if(b->parent != NULL){

        if(b->parent->right == a){

            b->parent->right = b;

        } else {

            b->parent->left = b;

        }
    }

    setBalance(a);
    setBalance(b);

    return b;
}

//Rotates node to the left to rebalance the tree and then calls to rotate it right for rebalancing
TLDNode* rotateLeftThenRight(TLDNode *n){

    n->left = rotateLeft(n->left);

    return rotateRight(n);

}

//Rotates node to the right to rebalance the tree and then calls to rotate it left for rebalancing
TLDNode* rotateRightThenLeft(TLDNode *n){

    n->right = rotateRight(n->right);

    return rotateLeft(n);

}

//Function to rebalance the avl tree, determines what nodes need to be rotated in which direction based upon their
//balance value
void rebalance(TLDList *tld, TLDNode *n){

    setBalance(n);

    if(n->balance == -2) {

        if (get_height(n->left->left) >= get_height(n->left->right)) {

            n = rotateRight(n);

        } else {

            n = rotateLeftThenRight(n);

        }
    }else if(n->balance == 2) {

        if (get_height(n->right->right) >= get_height(n->right->left)) {

            n = rotateLeft(n);

        } else {

            n = rotateRightThenLeft(n);

        }

    }

    //Checks if balanced node is now the root if so updates the root, if not rebalances the parent node
    if (n->parent != NULL){

        rebalance(tld, n->parent);

    }else{

        n->parent = NULL;
        tld->root = n;

    }

}

//Used for once the itertaor is done with iterating through nodes returning their found variables back to false to
// allow them to be iterated again
void tlditer_node_unfinder(TLDNode *n){

    n->found = false;

    if(n->right != NULL){

        tlditer_node_unfinder(n->right);

    }
    if(n->left != NULL){

        tlditer_node_unfinder(n->left);

    }

}

// CReates an instance of the iterator
TLDIterator *tldlist_iter_create(TLDList *tld){

    struct tlditerator *p;

    //Gets memory for the iterator if so initialises it
    if ((p = (struct tlditerator *) malloc(sizeof(struct tlditerator))) != NULL) {

        p->root = tld->root;

        return p;

    }else{

        return NULL;

    }

}

//Function that adds new nodes to the avl tree

int tldlist_add(TLDList *tld, char *hostname, Date *d){

    //Initiliaze variables for function to use
    char delim[] = ".";
    char *ptr = strtok(hostname, delim);
    char *ptr2 = strtok(NULL, delim);
    bool search_add = false;
    bool search_add_success = false;

    //loops through the hostname to get the ending tld
    while(ptr2 != NULL){

        ptr = ptr2;

        ptr2 = strtok(NULL, delim);

    }

    //Checks if the given data is within the search dates
    if((date_compare(tld->begin_date, d)<1) && ((date_compare(tld->end_date, d)>-1))){

        struct tldnode *p;

        //checks if there is a tree root, if no create a new node and initialise it as the root
        if(tld->root == NULL){

            if ((p = (struct tldnode *)malloc(sizeof(struct tldnode))) != NULL) {

                p->parent = NULL;
                p->TLDcount = 1;
                strcpy(p->TLDstr, ptr);
                p->left = NULL;
                p->right = NULL;
                p->height= 0;
                p->found = false;
                tld->root = p;
                tld->add_success +=1;
                tld->TLDSize+=1;
                search_add_success = true;
                search_add = true;

            }else{

                //if node creation fails need to exit while loop to ensure we don't loop forever
                search_add = true;

            }

        }else{

            p = tld->root;

            struct tldnode *q;

            while (search_add == false){

                //If provided tld is in the avl update the node by increasing the count
                if(strcmp(ptr, p->TLDstr) == 0){

                    p->TLDcount += 1;
                    tld->add_success +=1;
                    search_add_success = true;
                    search_add = true;

                //Determine if need to go left or right in avl tree
                }else if(strcmp(ptr, p->TLDstr) > 0){

                    if(p->right == NULL){

                        //create new node if it doesn't exist, if new node successful initialise it
                        if ((q = (struct tldnode *)malloc(sizeof(struct tldnode))) != NULL){

                            q->parent = p;
                            q->TLDcount = 1;
                            strcpy(q->TLDstr, ptr);
                            q->left = NULL;
                            q->right = NULL;
                            q->height = 0;
                            q->found = false;
                            p->right = q;
                            rebalance(tld, p);
                            tld->add_success +=1;
                            tld->TLDSize+=1;
                            search_add_success = true;
                            search_add = true;

                        }else{

                            //if node creation fails need to exit while loop to ensure we don't loop forever
                            search_add = true;


                        }


                    } else{

                        p = p->right;

                    }

                }else {

                    if (p->left == NULL) {

                        //create new node if it doesn't exist, if new node successful initialise it
                        if ((q = (struct tldnode *) malloc(sizeof(struct tldnode))) != NULL) {

                            q->parent = p;
                            q->TLDcount = 1;
                            strcpy(q->TLDstr, ptr);
                            q->left = NULL;
                            q->right = NULL;
                            q->height = 0;
                            q->found = false;
                            p->left = q;
                            rebalance(tld, p);
                            tld->add_success +=1;
                            tld->TLDSize+=1;
                            search_add_success = true;
                            search_add = true;

                        } else {

                            //if node creati0n fails need to exit while loop to ensure we don't loop forever
                            search_add= true;

                        }

                    } else {

                        p = p->left;

                    }

                }

            }

        }

    }

    //return 1 if node was successfully updated/created
    if(search_add_success == true){

        return 1;

    }

    return 0;

}

//returns the count of how many times tld's were successfully added to their nodes in avl tree
long tldlist_count(TLDList *tld){

    return tld->add_success;

}

//Used to get the next unvisited node returns null if none are found
TLDNode *tld_node_find(TLDNode *n){

    if(n->found == false){

        if(n->right != NULL && n->right->found != true){

            return tld_node_find(n->right);

        }else if(n->left != NULL && n->left->found != true){

            return tld_node_find(n->left);

        }else{

            n->found = true;
            return n;

        }

    }

    return NULL;

}

//Gets the next unfound node, if none found returns NULL, makes use of tld_node_find
//If all nodes been find resets the nodes to be unfound
TLDNode *tldlist_iter_next(TLDIterator *iter) {

    struct tldnode *p;
    p = iter->root;

    p = tld_node_find(p);

    if(p == NULL){

        tlditer_node_unfinder(iter->root);

    }

    return p;

}

//frees the malloc'd memory for the iterator
void tldlist_iter_destroy(TLDIterator *iter){

    free(iter);

}

//returns the tld name from the node that repreents it
char *tldnode_tldname(TLDNode *node){

    return node->TLDstr;

}

//returns the count of tld visits on that node
long tldnode_count(TLDNode *node){

    return node->TLDcount;

}

//Frees the memory the node takes up
void tldnode_destroy(TLDNode *n){

    free(n);

}

//frees the memory tldlist uses, to do so it creates an iterator and iterates over the nodes freeing them,
//upon completion frees the iterator and then frees the tldlists
void tldlist_destroy(TLDList *tld){

    struct tlditerator *iter;
    struct tldnode *n;

    iter = tldlist_iter_create(tld);

    while ((n = tldlist_iter_next(iter))) {
        free(n);
    }

    tldlist_iter_destroy(iter);

    free(tld);

}















