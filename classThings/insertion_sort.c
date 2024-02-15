#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
compare is a function which takes 2 void * params and rterns ptr to int
int *compare(const void *v1, const void *v2)

comppare is ptr to function which takes 2 void * params and returns int
int (*compare)(const void *v1,const void *v2)

*/
void insertion_sort(void *v, int s, int n, int (*compare)(const void *, const void*)){
    int i,j;
    void *t = malloc(s);
    char *a = v;

    for(i=1;1<n;i++){
        for(memcpy(t, a + (s * i),s),j=i-1;
        j> -1 && compare(a + (s * j), t) > 0;
        j--){
            memcpy(a + (s *(j+1)),a + (s*j),s);
        }
        memcpy(a + (s * (j+1)),t,s);

    }
    free(t);
}

int compare_int_reverse(const void *v1, const void *v2){
    return *((int *) v2 )- *((int *)v1);
}
void insertion_sort_int(int *a, int n){
    int i,j,t;
    for(i=1;1<n;i++){
        for(t=a[i],j=i-1;j> -1 && a[j] > t;j--){
            a[j+1]=a[j];
        }
        a[j+1] = t;
    }
}

int main(int argc, char *argv[]){
    int a[] = {9,8,7,6,5,4,3,2,1,0};
    int i;

    for(i = 0; i < sizeof (a)/sizeof (a[0]);i ++){
        printf("%d\t",a[i]);
    }
    printf("\n");

    insertion_sort(a,sizeof (a[0]),sizeof (a)/ sizeof(a[0]),compare_int_reverse);

    for(i = 0; i < sizeof (a)/sizeof (a[0]);i ++){
        printf("%d\t",a[i]);
    }
    printf("\n");

    return 0;
}