#include <stdlib.h>
#include "stack.h"

int stack_init(struct stack *s){
    s->top = NULL;
    s->size = 0;
    return 0;
}
int stack_destroy(struct stack *s){
    struct stack_node *n;
    for(n = s->top;n;n=s->top){
        s->top = s->top->next;
        free(n);
    }
    //not necessary
    s->size = 0;
    return 0;
}
int stack_push(struct stack *s, int d){
    struct stack_node *n;
    if(!(n = malloc(sizeof(*n)))){
        return 1;
    }
    n->data = d;
    n->next = s->top;
    s->top = n;
    s->size++;
    return 0;
}
int stack_pop(struct stack *s, int *d){
    struct stack_node *n;
    if(!s->size){
        return 1;
    }
    *d = s->top->data;
    n = s->top;
    s->top = s->top->next;
    free(n);
    s->size--;
    return 0;
}
int stack_top(struct stack *s, int *d){
    if(!s->size){
        return 1;
    }
    *d = s->top->data;
    return 0;
}
int stack_size(struct stack *s){
    return s->size;
}
int stack_is_empty(struct stack *s){
    return !s->size;
}