#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void part1()
{
    FILE *menu;
    FILE *reciept;
    
    char holder;
    int kontrol=1;
    int choice;
    int servings;
    double price=0;
    double fiyat;
     
    time_t t = time(NULL);              //Writing the current time with strftime function
    struct tm *tm = localtime(&t);
    char currenttime[64];
    strftime(currenttime,sizeof(currenttime),"%d.%m.%Y/%H:%M", tm);
   
    
   
    
   menu=fopen("menu.txt", "r");
   reciept=fopen("reciept.txt", "w");
   fprintf(reciept,"220104004912   %s\n\n----------------------------------\n\nProduct                Price(TL)\n\n----------------------------------\n\n",currenttime);
  // PRINT THE MENU
 
 while((holder=fgetc(menu)) && holder != EOF)  //Printing the menu
   { 
     fscanf(menu,"%d",&choice);
     printf("%c",holder);
   }
 
   fclose(menu);
   printf("\n");
   
 // SELECTION OF THE FOOD
 while(kontrol)   
 {   
   printf("Enter your choice: ");
   scanf("%d",&choice);
   
   if(choice==1)            //Actions to take if the user requests a food from the first line
   { 
      printf("How many servings do you want?: ");
      scanf("%d",&servings);
      menu=fopen("menu.txt", "r");
      if(servings>1)
      fprintf(reciept,"%d* ",servings);
       
       while((holder=fgetc(menu)) && (holder != ' ' )) { 
        
        fprintf(reciept,"%c",holder);
       }
       fclose(menu);
       menu=fopen("menu.txt", "r");
      while((holder=fgetc(menu)) && (holder != '\n' ))
      fscanf(menu,"%lf",&fiyat);
     
     fprintf(reciept,"              %g\n\n",servings*fiyat); 
     price=price+fiyat*servings;
     fclose(menu);
   }
  else if(choice==0)
  {
   kontrol=0;
  }  
  else         //Actions to take if the user requests a food from other than first line        
  {  
     printf("How many servings do you want?: ");
     scanf("%d",&servings);
     menu=fopen("menu.txt", "r");
  
     int line=0;               //Function that allows jumping to that line to get the name information of the food on the line the user wants
      while((holder=fgetc(menu)) != EOF) {    
       if(holder == '\n') line++;
       if(line == choice-1) break; 
     }
    if(servings>1)
    fprintf(reciept,"%d* ",servings);
    
    while((holder=fgetc(menu)) && (holder != ' ' )) { 
        fprintf(reciept,"%c",holder);
    }
    
    fclose(menu);
    menu=fopen("menu.txt", "r");
    
     line=0;                 //Function that allows jumping to that line to get the price information of the food on the line the user wants
      while((holder=fgetc(menu)) != EOF) {
       if(holder == '\n') line++;
       if(line == choice-1) break; 
     }
    while((holder=fgetc(menu)) && (holder != '\n' ))
    fscanf(menu,"%lf",&fiyat);
    
    fprintf(reciept,"              %g\n\n",servings*fiyat);
    price=price+fiyat*servings;
    fclose(menu);
  
   }
  }  
    fprintf(reciept,"Total:                  %g\n\n",price);
   

  //Calculation of the price and discounts
  char student;
  printf("Are you a student ? (Y/N): ");
  scanf(" %c",&student);
  
  if(student=='Y')
  {
   fprintf(reciept,"Student discount:       %g\n\n",price*0.125);
   if(price>=150)
   {
     fprintf(reciept,"Discount:               %g\n\n",price*0.1);
     price=price-price*0.1-price*0.125;
   }
   else
   price=price-price*0.125;
  }
  else if(student=='N' && price>=150)
  {
   fprintf(reciept,"Discount: %g\n\n",price*0.1);
   price=price*0.9;
  }
  
  fprintf(reciept,"----------------------------------\n\nPrice:                  %g\n\n",price);
  fprintf(reciept,"Price + VAT:            %g\n",price*1.18);
  fclose(reciept);
  reciept=fopen("reciept.txt","r");
  
  printf("\n");
  while(!feof(reciept))
   {
     fscanf(reciept,"%c",&holder);
     printf("%c",holder);
  }
 fclose(reciept); 
} 




void part2()
{
   
   int choiceofuser,choiceofcomputer;
   char yes_no = 'Y';
   
 while(yes_no == 'Y')
 {
   srand(time(0));
   choiceofcomputer = rand()%3+1;     //We use to random select for the computer's selection
 
   printf("Please make a choice!\n 1: Stone, 2: Paper, 3: Scissors\n");
   scanf("%d",&choiceofuser);         //Selection of user
   
   if(choiceofuser==1 && choiceofcomputer==1)         //Probabilities in case the user chooses stone
   printf("You chose Stone. I chose Stone. It's a tie!\n");
   else if(choiceofuser==1 && choiceofcomputer==2)
   printf("You chose Stone. I chose Paper. I won!\n");
   else if(choiceofuser==1 && choiceofcomputer==3)
   printf("You chose Stone. I chose Scissors. You won!\n");
   
   if(choiceofuser==2 && choiceofcomputer==1)        //Probabilities in case the user chooses paper
   printf("You chose Paper. I chose Stone. You won!\n");
   else if(choiceofuser==2 && choiceofcomputer==2)
   printf("You chose Paper. I chose Paper. It's  a tie!\n");
   else if(choiceofuser==2 && choiceofcomputer==3)
   printf("You chose Paper. I chose Scissors. I won!\n");
   
   if(choiceofuser==3 && choiceofcomputer==1)        //Probabilities in case the user chooses scissors
   printf("You chose Scissors. I chose Stone. I won!\n");
   else if(choiceofuser==3 && choiceofcomputer==2)
   printf("You chose Scissors. I chose Paper. You won!\n");
   else if(choiceofuser==3 && choiceofcomputer==3)
   printf("You chose Scissors. I chose Scissors. It's a tie!\n");
   
   printf("Do you want to play again? (Y/N): ");
   scanf(" %c",&yes_no);
   if(yes_no=='N')
   break;
 }
}  

int main()
{
   part1();
   part2();

}




    
