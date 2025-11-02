#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define UP 'w'        //Definiton of arrow keys
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

 //Function of room walls
 void make_walls(int xofcharacter,int yofcharacter,int xofdoor,int yofdoor,int roomsize)   
{  
   
   system("clear");    //Clear the screen after every move
   int xofplane=0;     //Coordinates of plane for imaginary coordinate system 
   int yofplane=0;
   
   for(int i=0; i<2*roomsize+1; i++)                               //Prints upper wall
   printf("-");
   printf("\n");
    
   for(int i=0; i<roomsize; i++)                                   //Prints interior wall 
   {  
      yofplane++;                                                 //Determines new lines coordinates. First new line is y=1
      for(int i=0; i<roomsize+1; i++)
      {   
         xofplane++;                                             //Determines x coordinates. First space is x=1
         if(xofplane==xofdoor && yofplane==yofdoor)
         {
             printf("|D");                                     //Printf the door
         }
         else if (xofplane==xofcharacter && yofplane==yofcharacter)
         {
             printf("|C");                                    //Print the character
         }
         else
         {
             printf("| ");
         }
          
      } 
      xofplane=0;        //Resets x value before new line          
      printf("\n");
   }
    
    for(int i=0; i<2*roomsize+1; i++)        //Prints lower wall
   printf("-");
   printf("\n");
   

} 



int control_status(int xofcharacter,int yofcharacter,int xofdoor,int yofdoor)   //Control fuction return 1 when coordinates of character are equal to coordinates of the door.
 { 
    if(xofcharacter==xofdoor && yofcharacter==yofdoor)
    return 1;
    else
    return 0;
 }




void moving_func(int xofcharacter, int yofcharacter, int xofdoor, int yofdoor, int roomsize)
{
  
  int kontrol=1;
  char move;
  int countofmove=0;
  
  
  while(kontrol)
  {
     printf("Enter your move: ");
     scanf("%c",&move);
    
     switch(move)
   {
      case UP: 
       yofcharacter--;                                                     //Since the y-axis value decreases upwards in the imaginary coordinate system, if up is selected, y decreases by one.
       make_walls(xofcharacter,yofcharacter,xofdoor,yofdoor,roomsize);
       break;
      case DOWN: 
       yofcharacter++;                                                    //Since the y-axis value decreases upwards in the imaginary coordinate system, if down is selected, y increases by one.
       make_walls(xofcharacter,yofcharacter,xofdoor,yofdoor,roomsize);
       break;
      case RIGHT: 
       xofcharacter++;
       make_walls(xofcharacter,yofcharacter,xofdoor,yofdoor,roomsize);   //If right is selected, x increases by one.
       break;
      case LEFT: 
       xofcharacter--;                                                  //If left is selected, x decreases by one.
       make_walls(xofcharacter,yofcharacter,xofdoor,yofdoor,roomsize);
       break;
   }
     countofmove++;
     
     if(xofcharacter==0)                             //If character coordinates  are equal to left,right,upper and lower walls printfs warning message. 
     {
        printf("YOU HIT THE WALL!!!!");
        
     }
     else if(xofcharacter==roomsize+1)
     {
        printf("YOU HIT THE WALL!!!!");
     }
     else if (yofcharacter==0)
     {
        printf("YOU HIT THE WALL!!!!");
        
     }
     else if (yofcharacter==roomsize+1)
     {
        printf("YOU HIT THE WALL!!!!");
     }



     if (control_status(xofcharacter,yofcharacter,xofdoor,yofdoor)==1)   //Calls control function after every move
     {
        kontrol=0;                                                       //If control function returns 1 kontrol integer returns 0 and while loop stops.
        printf("Count of move: %d\n",countofmove);
        printf("Amazing!!\n");
     }
     

   
  }
}

 
 
 int menu()
 {   
     int xofcharacter;
     int yofcharacter;
     int xofdoor;
     int yofdoor;
     int choice;
     int roomsize;
     int roomsizecontrol=1;
  while(choice!=3)
  {
     printf("1. New Game\n");
     printf("2. Help\n");
     printf("3. Exit\n");
     printf("Your choice: ");
     scanf("%d",&choice);
     
     if(choice==1)
     {
       printf("Plese enter room size: ");
       scanf("%d",&roomsize);
       if(roomsize<5 || roomsize>10)                      
      {
        printf("Invalid roomsize! Please enter between 1-10\n");
        roomsizecontrol=0;
       } 
      else
      { 
       srand(time(0));                                  //Determines coordinates of the door and character randomly.  
       xofcharacter= (rand()% roomsize) + 1;
       yofcharacter= (rand()% roomsize) + 1;
       xofdoor= (rand()% roomsize) + 1;
       yofdoor= (rand()% roomsize) + 1;
      make_walls(xofcharacter,yofcharacter,xofdoor,yofdoor,roomsize);
      moving_func(xofcharacter,yofcharacter,xofdoor, yofdoor, roomsize);
     }
     
     
      
     }  
     else if(choice==2)      //Help section
     {
       printf("The character is able to move one space in any of the four cardinal directions: up, down, left, and right. Diagonal moves are not allowed. ");
       printf("The user will input their desired move using the following keys: 'a' for left, 'd' for right, 'w' for up, and 's' for down. ");
       printf("If  the  character  attempts  to  move  through  a  wall,  a  warning  message  should  be  displayed.  The game ends when the character reaches the door.\n");
     }
    
    } 
    
     
 }
 
 

int main()
{  
   menu();        //Calls menu then the menu function call the make walls function and moving function then moving function calls control funciton.
  
}














