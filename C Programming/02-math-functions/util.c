#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"


void part1()
{
  int year;
  printf("Pleas enter a year: ");
  scanf("%d",&year);
  
  if(year%4==0 && year%100!=0)                 //The exception of leap years is years that are divisible by 100 but not exactly divisible by 400. 
   printf("%d is a leap year.\n",year);        //Since these years are not considered leap years, the code is written accordingly.
   
  else if (year%100==0 && year%400==0)
   printf("%d is a leap year.\n",year);
   
  else
   printf("%d is not a leap year.\n",year);

}



 //PART 2 PART 2 PART 2
void part2()

{
  double result;
  double firstoperand,secondoperand;
  char operator;
  char format;
  int n;
  int m;
  int numberforfact;  // An integer number is required for factorial 
  int kontrol=1;      // A kontrol integer is required to stop code when m value is less than n or less than number of digits of the result
  
  printf("*******************************\n");
  printf("Enter the format of output (S or I): ");    // Selection of the format
  scanf(" %c",&format);
  printf("Enter your operator (+,-,*,/,^,%%,!): ");   // Selection of the operation
  scanf(" %c",&operator);
  
  if(format=='S')
  {
    printf("Enter your your m and s values: ");      // If format is 'S' m and n values are required
    scanf(" %d %d",&m,&n);
      if(m<n)
    {                                                 
       printf("ERROR! M VALUES IS LESS THAN N !!\n");  // Warning when m i less than n and kontrol integer turn 0 in order to stop the code.
       kontrol=0;
    }
  }
  
  
  
  if(operator!='!' && kontrol)            // If the operation is not factorial two double number are required. We take them from the user in this part.
 {
  
  printf("Please enter first operand: ");
  scanf("%lf",&firstoperand);
  printf("Please enter second operand: ");
  scanf("%lf",&secondoperand);
 }
  else if (operator=='!' && kontrol)     // If the operation is factorial an integer number is required. We take it from the user in this part.
 {
  printf("Please enter an integer number: ");
  scanf("%d",&numberforfact);
 }  
  
  
 switch(operator){                    
  
  case '+':
    result=firstoperand+secondoperand;
    break;
  case '-':
    result=firstoperand-secondoperand;
    break;
  case '*':
    result=firstoperand*secondoperand;
    break;
  case '/':
    result=firstoperand/secondoperand;                      // We make the calculations according to selected operator by user.
    break;
  case '^':
    result=pow(firstoperand,secondoperand);
    break;
  case '%':
    result=fmod(firstoperand,secondoperand);
    break;
  case '!':
  
    if(numberforfact!=0 || numberforfact!=1)
    {
       result=1;
       for(int i=2; i<=numberforfact ; i++)
       {
        result=result*i;
       }
    }
    else if (numberforfact==1)
    {
     result=1;
    }
    else
    {
      result=0;
    }
    break;
  default:
    printf("Error! Your operator is invalid!\n"); 
    break;
  
  }
 
 
  int inum=result;       // We convert the double number to an integer in order to find number of digits before the comma. We use a while loop for this.
  int idigit=0;
  while(inum!=0){
  inum=inum/10;
  idigit++;  
  }
  
  if(format=='S' && fmod(result,1)!=0 && kontrol)    //When the result is not an integer (so mod of this number to 1 is not equal to 0 ) m value is should be greater or equal to idigit+n.                    
   {                       // idigit is number of digits before comma, n is number of digits after comma. When m<n+idigit a warning message will print and kontrol integer turns 0 in order to stop.
     if(m<idigit+n)
     { 
       printf("ERROR! M VALUES IS LESS THAN NUMBER OF DIGITS OF THE RESULT!!\n");
       kontrol=0;
     }
   }
  else if (format=='S' && fmod(result,1)==0 && kontrol )
   {                     //When the result is an integer (so mod of this number to 1 is equal to 0 ) m value is should be greater or equal to idigit.          
     if(m<idigit)        // idigit is number of digits before comma. When m<idigit a warning message will print and kontrol integer turns 0 in order to stop.  
     {                  
       printf("ERROR! M VALUES IS LESS THAN NUMBER OF DIGITS OF THE RESULT!!\n");
       kontrol=0;
     }
     
   }
  
 if(format=='S' && kontrol)        //When the user select 'S' and kontrol integer is 1 scientific format will print.
 {                                
    if(fmod(result,1)!=0)        //These two groups are separated by if statements, as the calculation of zeros to be added when the number is an integer or a double number has changed. 
   {
       if(result<1 && result>-1)  //If number is between -1 and 1 one less zero required.
       m=m-1;
       for(int i=0; i<m-idigit-n; i++)
       printf("0");
       printf("%.*lfe%d\n",n,result,0); 
   }
   else 
   {
      for(int k=0; k<m-idigit; k++)
      printf("0");
      printf("%.*lfe%d\n",n,result/pow(10,n),n);
  
   }
 }
 else if (format=='I' && kontrol)   //When the user select 'I' and kontrol integer is 1 normal format will print.
 printf("%g\n",result);
 
 printf("*******************************\n");
  
 }
 
 //PART 3 PART 3 PART 3
 
 void part3()
{   
    printf("********************************\n");
    
    double firstexam, secondexam, thirdexam, firstassignment, secondassignment;
    double finalgrade;
    int kontrol=1;
    
    printf("Enter 3 exam grades of student: ");
    scanf("%lf %lf %lf",&firstexam,&secondexam,&thirdexam);
    printf("Enter 2 assignment grades of student: ");
    scanf("%lf %lf",&firstassignment,&secondassignment);
    
    if(firstexam>100 || secondexam>100 || thirdexam>100 || firstassignment>100 || secondassignment>100)  // Code that will warn if any score entered is greater than one hundred
    {  
     kontrol=0;  
     printf("Please enter a valid grade!\n");         
    }
    
    finalgrade= (firstexam+secondexam+thirdexam)/3*0.6 + (firstassignment+secondassignment)/2*0.4; // Calculation of finalgrade;
    
    
    if(finalgrade<60 && kontrol)              // When final grades is below 60. 
    { 
     printf("Final Grade: %g ",finalgrade);   // When control integer is 0 the if statement doesn't work.
     printf("Failed!\n");
     
    }
    else if (finalgrade>=60 && kontrol)         // When final grades is above or equal to 60. 
    {        
     printf("Final Grade: %g ",finalgrade);     // When control integer is 0 the if statement doesn't work.
     printf("Passed!\n");
    }
    
    printf("********************************\n");




}


