#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct{
  char board[9];   //Game Table
    
}game;

 int control_status(game game_table){   //Control function if game is finished or not
 
 if(game_table.board[0] == '1' && game_table.board[1] == '2' && game_table.board[2] == '3' && game_table.board[3] == '4' && game_table.board[4] == '5' && game_table.board[5] == '6' && game_table.board[6] == '7' && game_table.board[7] == '8' && game_table.board[8] == '_')
  return 1;
 
  return 0;
 }

 void print_game_table(game game_table, int usedIndex[]){  //Printing game table to terminal and file
 FILE *fp;
  for(int i=0; i<9; i++){
    printf("%c ",game_table.board[i]);
    if(i%3==2)
    printf("\n");
  }
  fp=fopen("table.txt","a");
  fprintf(fp,"  ");
  for(int i=0; i<9; i++){
    fprintf(fp,"%c ",game_table.board[i]);
    if(i%3==2)
    fprintf(fp,"\n  ");
  }
  fprintf(fp,"\n");
  fclose(fp);
  
  
}


void moving_function(game game_table, int usedIndex[]){ //Moving function for user
 char choice,number_choice,temp;
 int a;
 char input[10];
 int score=1000;
 int count_move=0;
 int best_score=0;
 FILE *fp;
do{
 printf("Please enter your move (number-direction,e.g.,2-R):\n");   //Moves according to user's selection
 fgets(input, sizeof(input), stdin); 
 sscanf(input, "%c-%c", &number_choice, &choice);
 
 switch(choice){
  case 'R':
   for(a=0; a<9; a++){
    if(game_table.board[a] == number_choice){
     if(a%3==2){
      printf("%c-%c is an illegal move!!! Please make a legal move!!!\n",number_choice,choice);
      break;
     }
     if(game_table.board[a+1] != '_'){
      printf("%c-%c is an illegal move!!! Please make a legal move!!!\n",number_choice,choice);
      break;
     }
     temp = game_table.board[a+1];
     game_table.board[a+1] = game_table.board[a];
     game_table.board[a] = temp;
     count_move++;
     break;
    }
   }
   break;
   
   case 'L':
   for(a=0; a<9; a++){
    if(game_table.board[a] == number_choice){
     if(a%3==0){
      printf("%c-%c is an illegal move!!! Please make a legal move!!!\n",number_choice,choice);
      break;
     }
     if(game_table.board[a-1] != '_'){
      printf("%c-%c is an illegal move!!! Please make a legal move!!!\n",number_choice,choice);
      break;
     }
     temp = game_table.board[a-1];
     game_table.board[a-1] = game_table.board[a];
     game_table.board[a] = temp;
     count_move++;
     break;
    }
   }
   break;
   
   case 'U':
   for(a=0; a<9; a++){
    if(game_table.board[a] == number_choice){
     if(a%3==a){
      printf("%c-%c is an illegal move!!! Please make a legal move!!!\n",number_choice,choice);
      break;
     }
     if(game_table.board[a-3] != '_'){
      printf("%c-%c is an illegal move!!! Please make a legal move!!!\n",number_choice,choice);
      break;
     }
     temp = game_table.board[a-3];
     game_table.board[a-3] = game_table.board[a];
     game_table.board[a] = temp;
     count_move++;
     break;
    }
   }
   break;
   
   case 'D':
   for(a=0; a<9; a++){
    if(game_table.board[a] == number_choice){
     if(a==6 || a==7 || a==8){
      printf("%c-%c is an illegal move!!! Please make a legal move!!!\n",number_choice,choice);
      break;
     }
     if(game_table.board[a+3] != '_'){
      printf("%c-%c is an illegal move!!! Please make a legal move!!!\n",number_choice,choice);
      break;
     }
     temp = game_table.board[a+3];
     game_table.board[a+3] = game_table.board[a];
     game_table.board[a] = temp;
     count_move++;
     break;
    }
   }
   break;
   
  default:
  printf("%c-%c is an illegal move!!! Please make a legal move!!!\n",number_choice,choice);
 }
 print_game_table(game_table,usedIndex);

}while(control_status(game_table) != 1);
 score = score-10*count_move;             //Calculation the score
 if(control_status(game_table) == 1){
  printf("Congratulations! You finished the game.\n\nTotal number of moves: %d\nYour score: %d\n",count_move,score);
 }
 fp=fopen("best_score.txt","r");
 fscanf(fp,"%d",&best_score);     //Printing the score if it is the best score.
 fclose(fp);
 if(score>best_score){
  fp=fopen("best_score.txt","w");
  fprintf(fp,"%d",score);
 }
}

int control(int usedNumbers[],int count,int randomNumber){  
 for(int i=0; i<count; i++){
    if(usedNumbers[i] == randomNumber)
    return 1;
     
 }    
   return 0; 
}

int generateNumber(int usedNumbers[],int count, int size){ //Generating random number for mixing the table 
  int randomNumber;
  
  do{
    randomNumber = rand()%9;  
  }while(control(usedNumbers,count,randomNumber));
    
  return randomNumber;  
}

bool control_invalid_move(int pre_move, int cur_move) {    //This function allows the computer to automatically solve puzzles while avoiding successive opposing movements. In this way, the table is not restored and the stack is used efficiently.

    if ((pre_move == 2 && cur_move == 1) || (pre_move == 1 && cur_move == 2) || (pre_move == 3 && cur_move == 4) || (pre_move == 4 && cur_move == 3)) {
        return true;
    }
    return false;
}

void auto_finish(game game_table, int usedIndex[], int count, int pre_move) { //Auto finish function
    

    
    int cur_move = -1, i = 0;
    char temp;
    while (game_table.board[i] != '_') {
        i++;
    }

    do {
        cur_move = rand() % 4 + 1;
 } while (control_invalid_move(pre_move, cur_move) || (i == 0 && (cur_move == 1 || cur_move == 3)) || (i == 8 && (cur_move == 2 || cur_move == 4)) || (i < 3 && i > 0 && cur_move == 3) || (i < 8 && i > 5 && cur_move == 4));

    

    if (cur_move == 1) {
        printf("Computer move: %c-L\n",game_table.board[i-1]);
        temp = game_table.board[i - 1];
        game_table.board[i - 1] = game_table.board[i];
        game_table.board[i] = temp;
    } else if (cur_move == 2) {
        printf("Computer move: %c-R\n",game_table.board[i+1]);
        temp = game_table.board[i + 1];
        game_table.board[i + 1] = game_table.board[i];
        game_table.board[i] = temp;
    } else if (cur_move == 3) {
        printf("Computer move: %c-U\n",game_table.board[i-3]);
        temp = game_table.board[i - 3];
        game_table.board[i - 3] = game_table.board[i];
        game_table.board[i] = temp;
    } else if (cur_move == 4) {
        printf("Computer move: %c-D\n",game_table.board[i+3]); 
        temp = game_table.board[i + 3];
        game_table.board[i + 3] = game_table.board[i];
        game_table.board[i] = temp;
    }

    print_game_table(game_table, usedIndex);
    
    count++;
    
    
    if(control_status(game_table) == 1){
     printf("Computer finished the game.\nTotal number of computer moves: %d\n",count);
     return;
    }
    
    
    
    auto_finish(game_table, usedIndex, count, cur_move);
}

void create_table(int choice) {  //Create table and mix it
   
   game game_table={'1','2','3','_','4','5','7','8','6'};  //Table is initialized here but then it will be shuffled and random table will be created.
   int usedIndex[8];
   int count=0;
   char temp;
   int k,j;
   int control_count=0;
  while(count<9){
    int randomIndex = generateNumber(usedIndex,count,9);
    usedIndex[count] = randomIndex;
    count++;
       
   }
   for(int i=0; i<9; i++){  // Table is mixed
   temp = game_table.board[i];
   game_table.board[i] = game_table.board[usedIndex[i]];
   game_table.board[usedIndex[i]] = temp;
   
  }
 
  for(k=1; k<9; k++){  //Controlling if the table is solvable or not if not recreate the table
   for(j=k-1; j>=0; j--){
   if(game_table.board[k] != '_' && game_table.board[j] != '_' && game_table.board[k] < game_table.board[j] )
    control_count++;
   }
   
  }
  if(control_count%2==1){
   create_table(choice);
  }
  if(control_count%2==0){
   print_game_table(game_table,usedIndex);
   if(choice == 2)
   auto_finish(game_table, usedIndex,0,0);
   else if(choice == 1)
   moving_function(game_table,usedIndex);
  }
  
}

int main(){
 srand(time(NULL));
 int choice;
 int best_score;
 FILE *fp;
do{   //The menu
 printf("Welcome to 8-D Puzzle Game!\nPlease select an option:\n1. Play game as a user.\n2. Finished the game with pc.\n3. Show the best score.\n4. Exit.\n");
 scanf("%d",&choice);
 if(choice == 1 || choice == 2){
  create_table(choice);
 }
 else if(choice == 3){
  fp=fopen("best_score.txt","r");
  fscanf(fp,"%d",&best_score);
  printf("The best score is %d\n\n\n",best_score);
 }
 
 
}while(choice != 4);




}






































