#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef enum   // Enum for part2
{
  red=1,
  green,
  blue,
  yellow,
  orange

}renkler;

/////////////////////////////////////////////////////////////////////////
// PART 1 //
////////////////////////////////////////////////////////////////////////


void part1_conversion(char *holder)   // Letter Case Conversion function         
{
  if(*holder<=90 && *holder>=65)
  *holder=*holder+32;
}

void part1()
{
  
  FILE *fp;
  char holder;
  int count=0;
  int array[26];
  
  for(int i=97; i<=122; i++)    // Read and count function to read file and count letters
   {
      fp=fopen("text.txt","r");
      while((holder=fgetc(fp)) != EOF)
      {  
         part1_conversion(&holder);
         if(holder==i)
         count++;
      }
      array[i-97]=count;
      count=0;
      fclose(fp);
   }
   
  for(int i=65; i<=90; i++)  // Function to pull and write letter numbers from array
 {
   printf("%c: ",i);
   printf("%d\n",array[i-65]);
   
  }
 
}

/////////////////////////////////////////////////////////////////////////
// PART 2 //
////////////////////////////////////////////////////////////////////////


int part2_color(renkler firstcolor, renkler secondcolor)
{ 
  double colors_array[5][3] = {{1,0,0}, {0,1,0}, {0,0,1}, {0.5,0.5,0}, {0.5,0.4,0.2}};   // Array of vectors of colors
  double first_color_array[3];         
  double second_color_array[3];
  double final_array[3];               // Array of mixed color array
  double results_array[5];          //Array of euclidean distances of each mixed vector to the 5 primary color vectors
  double result=0;
  // Determining the first vector according to selected color
   if(firstcolor==red)         
  {
   first_color_array[0]=1.0;
   first_color_array[1]=0.0;
   first_color_array[2]=0.0;
  }
  else if(firstcolor==green)
  {
   first_color_array[0]=0;
   first_color_array[1]=1;
   first_color_array[2]=0;
  }
  else if(firstcolor==blue)
  {
   first_color_array[0]=0;
   first_color_array[1]=0;
   first_color_array[2]=1;
  }
  else if(firstcolor==yellow)
  {
   first_color_array[0]=0.5;
   first_color_array[1]=0.5;
   first_color_array[2]=0;
  }
  else if(firstcolor==orange)
  {
   first_color_array[0]=0.5;
   first_color_array[1]=0.4;
   first_color_array[2]=0.2;
  }
  
  // Determining the second vector according to selected color
  if(secondcolor==red)
  {
   second_color_array[0]=1;
   second_color_array[1]=0;
   second_color_array[2]=0;
  }
  else if(secondcolor==green)
  {
   second_color_array[0]=0;
   second_color_array[1]=1;
   second_color_array[2]=0;
  }
  else if(secondcolor==blue)
  {
   second_color_array[0]=0;
   second_color_array[1]=0;
   second_color_array[2]=1;
  }
  else if(secondcolor==yellow)
  {
   second_color_array[0]=0.5;
   second_color_array[1]=0.5;
   second_color_array[2]=0;
  }
  else if(secondcolor==orange)
  {
   second_color_array[0]=0.5;
   second_color_array[1]=0.4;
   second_color_array[2]=0.2;
  }
  
  
  //Calculation of mixed vector
  for(int i=0; i<3; i++)
  {
   final_array[i] = (first_color_array[i] + second_color_array[i])/2;
  }
  
  //Calculation of euclidean distances 
  for(int i=0; i<5; i++)
  {
     for(int k=0; k<3; k++)
     {
        result = result + pow((colors_array[i][k] - final_array[k]),2);
        
     }
     results_array[i] = sqrt(result);
     result=0;
  }
  
  //A function to find the minumum distance and the index of minumum distance in the results_array
  double min= results_array[0];
  int index_min;
  for(int i=0; i<5; i++)
  {
    if(results_array[i]<min)
    {
      min=results_array[i];
      index_min=i;
    }
  }
  
  //According the index of minumum distance we find the closest color vector to mixed vector
  if(index_min == 0)
  {
    return red;
  }
  else if(index_min == 1)
  {
    return green;
  }
  else if(index_min == 2)
  {
    return blue;
  }
  else if(index_min == 3)
  {
    return yellow;
  }
  else if(index_min == 4)
  {
    return orange;
  }
    
}
//The function that prints the closest color vector to mixed vector 
void part2_colorMixer(int (*pt)(renkler, renkler), renkler firstcolor, renkler secondcolor)  
{
 
   if(pt(firstcolor, secondcolor)==red)
  {
    printf("RED [1,0,0]\n");
  }
  else if(pt(firstcolor, secondcolor)==green)
  {
    printf("GREEN [0,1,0]\n");
  }
  else if(pt(firstcolor, secondcolor)==blue)
  {
    printf("BLUE [0,0,1]\n");
  }
  else if(pt(firstcolor, secondcolor)==yellow)
  {
    printf("YELLOW [0.5,0.5,0]\n");
  }
  else if(pt(firstcolor, secondcolor)==orange)
  {
    printf("ORANGE [0.5,0.4,0.2]\n");
  }
   
}

void part2()
{
//We want the colors from user and according to that we send the enum values to functions
  char fc,sc;
  renkler firstcolor, secondcolor;           
  printf("Enter first color (r,g,b,y,o): ");
  scanf("%c",&fc);
  
  if(fc == 'r')
  firstcolor= red;
  else if(fc == 'g')
  firstcolor= green;
  else if(fc == 'b')
  firstcolor= blue;
  else if(fc == 'y')
  firstcolor= yellow;
  else if(fc == 'o')
  firstcolor= orange;
  
  printf("Enter second color (r,g,b,y,o): ");
  scanf(" %c",&sc);
  
  if(sc == 'r')
  secondcolor= red;
  else if(sc == 'g')
  secondcolor= green;
  else if(sc == 'b')
  secondcolor= blue;
  else if(sc == 'y')
  secondcolor= yellow;
  else if(sc == 'o')
  secondcolor= orange;
  
  part2_colorMixer(&part2_color,firstcolor,secondcolor);
}

/////////////////////////////////////////////////////////////////////////
// PART 3 //
////////////////////////////////////////////////////////////////////////

//Function for pritning the game table
void part3_print_game(char array[3][3])    
{
   for(int i=0; i<3; i++)
   {
      for(int j=0; j<3; j++)
      {
        printf("%c ",array[i][j]);
      }
      printf("\n");
   }
}
//The function control the status of the game
int part3_control_status(char array[3][3])
{
   int control=0;
   char choice;
//We check the status in the rows if none of them are empty  
   for(int i=0; i<3; i++)
  {
    if(array[i][0] != '_' && array[i][1] != '_' && array[i][2] != '_')
    {
       if(array[i][0] == array[i][1] && array[i][1] == array[i][2])
       return 1;
    }
  }
//We check the status in the columns if none of them are empty  
  for(int i=0; i<3; i++)
  {  
    if(array[0][i] != '_' && array[1][i] != '_' && array[2][i] != '_')
    {
      if(array[0][i] == array[1][i] && array[1][i] == array[2][i])
      return 1;
    }
  } 
//We check the status in diagonals if none of them are empty
   if(array[0][0] != '_' && array[1][1] != '_' && array[2][2] != '_')
    {
      if(array[0][0] == array[1][1] && array[1][1] == array[2][2])
      return 1;
    }
    
    if(array[0][2] != '_' && array[1][1] != '_' && array[2][0] != '_')
    {
      if(array[0][2] == array[1][1] && array[1][1] == array[2][0])
      return 1;
    }
//We check probability of the draw if none of the blocks are empty
  for(int i=0; i<3; i++)
  {
    for(int k=0; k<3; k++)
    {
      if(array[i][k]=='_')
      control=1;
    }
  } 
  if(control==0)
  {
   printf("It's a draw!\n");
   return 0;
  } 
} 

void part3()
{
  char choice = 'Y';
  int xof1,yof1,xof2,yof2;
  char array[3][3]= {{'_','_','_'},{'_','_','_'},{'_','_','_'}};
  
  part3_print_game(array);

while (choice == 'Y')
{
  while(1)
  {
    printf("Player 1 (X) enter your move (row,col): ");  //Moves of X's
    scanf("%d %d",&xof1,&yof1);
    array[xof1][yof1] = 'X';
    part3_print_game(array);
    
    if(part3_control_status(array) == 1)     //Control status after each move
    {
       printf("Player 1 (X) wins!\n");
       break;
    }
    else if(part3_control_status(array) == 0)
    break;
    
    printf("Player 2 (O) enter your move (row,col): ");  //Moves of Y's 
    scanf("%d %d",&xof2,&yof2);
    array[xof2][yof2] = 'O';
    part3_print_game(array);
    
    
    if(part3_control_status(array) == 1)    //Control status after each move
    {
       printf("Player 2 (O) wins!\n");
       break;
    }
    else if(part3_control_status(array) == 0)
    break;
   
   } 
   
   
  printf("Do you want to play the game again? (Y/N): ");
  scanf(" %c",&choice);
  if(choice == 'Y')    //If user wants to play again we clear the game board
  {
    for(int i=0; i<3; i++)       
    {
        for(int k=0; k<3; k++)
        array[i][k] = '_';
    }
    part3_print_game(array);
        
    }
  else
  break;
  
}    
     
}


int main()
{ 
  printf("PART 1\n\n");
  part1();
  printf("\n\nPART 2\n\n");
  part2();
  printf("\n\nPART 3\n\n");
  part3();
  return 0;   
}
