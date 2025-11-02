#include <stdio.h>
#include "util.h"

void fraction_print(int numerator, int denominator) {
    printf("%d//%d\n", numerator, denominator);
   
    
}  /* end fraction_print */

void fraction_add(int n1, int d1, int n2, int d2, int * n3, int * d3) {
    *n3 = n1*d2 + n2*d1;
    *d3 = d1*d2;
    fraction_simplify(n3, d3);
} /* end fraction_add */

void fraction_sub(int n1, int d1, int n2, int d2, int * n3, int * d3) {
    *n3= n1*d2-n2*d1;
    *d3= d1*d2;
    fraction_simplify(n3, d3);
} /* end fraction_sub */

void fraction_mul(int n1, int d1, int n2, int d2, int * n3, int * d3) {
    *n3= n1*n2;
    *d3= d1*d2;
    fraction_simplify(n3, d3);
} /* end fraction_mul */

void fraction_div(int n1, int d1, int n2, int d2, int * n3, int * d3) {
    *n3= n1*d2;
    *d3= d1*n2;
    fraction_simplify(n3, d3);
} /* end fraction_div */

/* Simplify the given fraction such that they are divided by their GCD */
void fraction_simplify(int * n, int * d) {
    int min;
    int gcd;
    if(*n<*d)
    {  
       min=*n;
    }
    else
    {
       min=*d;
    }
    
    for(int i=0; i<min; i--)
    {
      if(*n%i==0 && *d%i==0)
      {
         gcd=min;
      }
    }
    
    *n=*n/gcd;
    *d=*d/gcd;
    printf("Result after simplify: %d//%d", *n, *d);
    
   
    
    
} /* end fraction_div */
