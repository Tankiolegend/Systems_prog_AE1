#include "tldlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//
// Created by James Ross on 29/10/2020.
//
struct tldlist {

    struct date *begin_date;
    struct date  *end_date;
    long add_success;
    struct tldnode *root;
    int TLDSize;

};

struct tldnode {

    char TLDstr[3];
    long TLDcount;
    struct tldnode *parent;
    struct tldnode *left;
    struct tldnode *right;
    int balance;
    int height;

};

struct tlditerator {

    struct tldnode *ptrs;
    int next_node;
    long max_node;


};



TLDList *tldlist_create( Date *begin,  Date *end){

    struct tldlist *p;

    if ((p = (struct tldlist *)malloc(sizeof(struct tldlist))) != NULL){

        p->begin_date = begin;
        p->end_date = end;
        p->root = NULL;
        p->TLDSize = 0;
        p->add_success = 0;

    }

    return p;

}

int get_height(TLDNode *n){

    if(n == NULL){

        return -1;

    }

    return n->height;

}

void reheight(TLDNode *n){

    if(n != NULL){

        if(get_height(n->right) > get_height(n->left)){

            n->height = 1 + get_height(n->right);

        }else{

            n->height = 1 + get_height(n->left);

        }


    }

}

void setBalance(TLDNode *n){

    reheight(n);

    n->balance = get_height(n->right) - get_height(n->left);

}


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

TLDNode* rotateLeftThenRight(TLDNode *n){

    n->left = rotateLeft(n->left);

    return rotateRight(n);

}

TLDNode* rotateRightThenLeft(TLDNode *n){

    n->right = rotateRight(n->right);

    return rotateLeft(n);

}

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

    if (n->parent != NULL){

        rebalance(tld, n->parent);

    }else{

        n->parent = NULL;
        tld->root = n;

    }

}


void tlditer_node_finder(TLDIterator *iter, TLDNode *n){

    iter->ptrs[iter->next_node] = *n;
    iter->next_node +=1;

    if(n->right != NULL){

        tlditer_node_finder(iter, n->right);

    }
    if(n->left != NULL){

        tlditer_node_finder(iter, n->left);

    }

}

TLDIterator *tldlist_iter_create(TLDList *tld){

    struct tlditerator *p;

    if ((p = (struct tlditerator *) malloc(sizeof(struct tlditerator))) != NULL) {

        p->ptrs = (struct tldnode*)malloc(tld->TLDSize * sizeof(struct tldnode*));
        p->next_node = 0;
        p->max_node = tld->TLDSize;

        tlditer_node_finder(p, tld->root);
        p->next_node = 0;

        return p;

    }else{

        return NULL;

    }

}

int tldlist_add(TLDList *tld, char *hostname, Date *d){

    char delim[] = ".";
    char *ptr = strtok(hostname, delim);
    char *ptr2 = strtok(NULL, delim);

    while(ptr2 != NULL){

        ptr = ptr2;

        ptr2 = strtok(NULL, delim);

    }

    if((date_compare(tld->begin_date, d)<1) && ((date_compare(tld->end_date, d)>-1))){

        struct tldnode *p;

        if(tld->root == NULL){

            if ((p = (struct tldnode *)malloc(sizeof(struct tldnode))) != NULL) {

                p->parent = NULL;
                p->TLDcount = 1;
                strcpy(p->TLDstr, ptr);
                p->left = NULL;
                p->right = NULL;
                p->height= 0;
                tld->root = p;
                tld->add_success +=1;
                tld->TLDSize+=1;
                return 1;

            }else{

                return 0;

            }

        }else{

            bool search_add = false;

            p = tld->root;

            struct tldnode *q;

            while (search_add == false){

                if(strcmp(ptr, p->TLDstr) == 0){

                    p->TLDcount += 1;
                    tld->add_success +=1;
                    printf("%d \n", tld->add_success);
                    search_add = true;

                    return 1;

                }else if(strcmp(ptr, p->TLDstr) > 0){

                    if(p->right == NULL){

                        if ((q = (struct tldnode *)malloc(sizeof(struct tldnode))) != NULL){

                            q->parent = p;
                            q->TLDcount = 1;
                            strcpy(q->TLDstr, ptr);
                            q->left = NULL;
                            q->right = NULL;
                            q->height = 0;
                            p->right = q;
                            rebalance(tld, p);
                            tld->add_success +=1;
                            tld->TLDSize+=1;

                            search_add = true;
                            return 1;

                        }else{

                            return 0;


                        }


                    } else{

                        p = p->right;

                    }

                }else {

                    if (p->left == NULL) {

                        if ((q = (struct tldnode *) malloc(sizeof(struct tldnode))) != NULL) {

                            q->parent = p;
                            q->TLDcount = 1;
                            strcpy(q->TLDstr, ptr);
                            q->left = NULL;
                            q->right = NULL;
                            q->height = 0;
                            p->left = q;
                            rebalance(tld, p);
                            tld->add_success +=1;
                            tld->TLDSize+=1;
                            search_add = true;
                            return 1;

                        } else {

                            return 0;

                        }

                    } else {

                        p = p->left;

                    }

                }

            }

        }

    }

    return 0;

}

long tldlist_count(TLDList *tld){

    return tld->add_success;

}




TLDNode *tldlist_iter_next(TLDIterator *iter) {

    struct tldnode *p;

    if (iter->next_node < iter->max_node) {

        p = &iter->ptrs[iter->next_node];

        iter->next_node += 1;
    }else{

        p = NULL;

    }

    return p;

}

void tldlist_iter_destroy(TLDIterator *iter){

    free(iter);

}

char *tldnode_tldname(TLDNode *node){

    return node->TLDstr;

}

long tldnode_count(TLDNode *node){

    return node->TLDcount;

}

void tldnode_destroy(TLDNode *n){

    free(n);

}

void tldlist_destroy(TLDList *tld){

    free(tld);

}















