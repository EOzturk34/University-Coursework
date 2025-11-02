#include <stdio.h>
#include "util.h"
void part1()
{  
   printf("\nPART 1:\n");
   // Required integers. Kontrol integer for while loop. 
   int firstnum;        
   int secondnum;
   int result;
   int kontrol=1;
   int min,max,temp;
   printf("Please enter your first number:");
   scanf("%d",&firstnum);
   printf("Please enter your second number:");
   scanf("%d",&secondnum);
   
   // Determining which number is small
   if(firstnum>secondnum)
   {
   min = secondnum;
   max=firstnum;
   }
   else
   {
   min=firstnum;
   max=secondnum;
   }
   temp = max%min;

  while(temp!=0)
   {
         max=min;
         min=temp;
         temp=max%min;
   }
   
   result=min;
   
   printf("GCD: %d\n",result);
   }

void part2()
 {
   printf("\nPART 2:\n");
   int firstnum;
   int secondnum;
   printf("Please enter your first number:");
   scanf("%d",&firstnum);
   printf("Please enter your second number:");
   scanf("%d",&secondnum);
   int result=firstnum+secondnum;
   
   printf("%6.d\n",firstnum);
   printf("%6.d\n",secondnum);
   printf("+\n");
   printf("------\n");
   printf("%6.d\n",result);
   
 }

void part3()
 {
   printf("\nPART 3:\n"); 
   int firstnum;
   int secondnum;
   printf("Please enter your first number:");
   scanf("%d",&firstnum);
   printf("Please enter your second number:");
   scanf("%d",&secondnum);
   int result=firstnum*secondnum;
   
   printf("%8.d\n",firstnum);
   printf("%8.d\n",secondnum);
   printf("*\n");
   printf("----------\n");

   
   // The 1s next to 8,7 and 6 are to make the zero print out if one of the intermediate results is zero.
   printf("%8.1d\n",firstnum*(secondnum%10));
   secondnum=secondnum/10;
   printf("%7.1d\n",firstnum*(secondnum%10));
   secondnum=secondnum/10;
   printf("%6.d\n",firstnum*(secondnum%10));
   printf("+\n");
   printf("----------\n");
   printf("%8.d\n",result);  
     
   }
 
  void part4()
  {  
     printf("\nPART 4:\n");
     int a;
     printf("Please enter a nubmer between 1-10: ");
     scanf("%d",&a);
     
     if(a>5 && a<=10)
     printf("The integer you entered is greater than 5.\n");
     else if(a<=5 && a>=1)
     printf("The integer you entered is less than or equal to 5\n");
     else
     printf("Invalid output\n");
  
  
  }
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 


