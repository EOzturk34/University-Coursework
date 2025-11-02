#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct { //Snake coordinates struct
    int x;
    int y;
} snake_point;

typedef struct { //Board type and value struct
    char type;
    int value;
} Block;

Block*** init_board() { //Initiliaze function
    srand(time(NULL));
    int i, j, obx, oby, bax, bay; //x,y coordinates for obstacle and bait
    Block*** board = (Block***)malloc(10 * sizeof(Block**));
    for (i = 0; i < 10; i++) {
        board[i] = (Block**)malloc(10 * sizeof(Block*));
        for (j = 0; j < 10; j++) {
            board[i][j] = (Block*)malloc(sizeof(Block));
            board[i][j]->type = 'e';
            board[i][j]->value = 0;
        }
    }
    do {
        obx = rand() % 9+1;  
        oby = rand() % 9+1;
        bax = rand() % 9+1;
        bay = rand() % 9+1;
    } while (obx == bax && oby == bay);

    for (i = 0; i < 10; i++) { //Initiliaze the board
        for (j = 0; j < 10; j++) {
            if (obx == i && oby == j) {
                board[i][j]->type = 'o';
                board[i][j]->value = rand()%9+1;
            } else if (bax == i && bay == j) {
                board[i][j]->type = 'b';
                board[i][j]->value = 0;
            }
        }
    }

    return board; //Return board as you want
}

Block*** draw_board(Block*** board, snake_point* snake_body, int length) { // Drawing function
    int i, j, snake_part_index = 0,control=1;
    for (i = 0; i < 12; i++) {   //Upper wall
        printf("-");
    }
    printf("\n");
    for (i = 0; i < 10; i++) {
        printf("|"); //Side walls
        for (j = 0; j < 10; j++) {
            control=1;
            for (snake_part_index = 0; snake_part_index < length; snake_part_index++) { //If function comes to one of the snake part's coordinates printf the part of the snake
                if (snake_body[snake_part_index].x == i && snake_body[snake_part_index].y == j) {
                    if (snake_part_index == 0) {
                        printf("O");
                        control=0;
                        break;
                        
                    } else {
                        printf("X");
                        control=0;
                        break;
                        
                    }
                   
                }
                
                
            }
            for (snake_part_index = 0; control && snake_part_index < length; snake_part_index++) { //Printing bait,obstacles and spaces
            if (snake_body[snake_part_index].x != i || snake_body[snake_part_index].y != j) {
                if (board[i][j][0].type == 'b') {
                    printf(".");
                    break;
                } 
                else if (board[i][j][0].type == 'o') {
                   printf("%d", board[i][j][0].value);
                   break;
                } else if (board[i][j][0].type == 'e') {
                    printf(" ");
                    break;
                }
            }
            }
            
            
        }
        printf("|\n"); //Side walls
    }
    for (i = 0; i < 12; i++) {
        printf("-");
    }
    printf("\n");

    return board;
}

int check_status(Block*** board, snake_point* snake_body, int length){ // Chechk the status and if game is over it returns 1.
 int i;
 if(snake_body[0].x == -1 || snake_body[0].y == -1 || snake_body[0].x == 10 || snake_body[0].y == 10 ){ //For hitting walls
  printf("GAME OVER!\nYou hit a wall.\n");
  return 1;
 }
 if(board[snake_body[0].x][snake_body[0].y]->type == 'o' && board[snake_body[0].x][snake_body[0].y]->value >= length){ //For encountered an obstacle greater than or equal to length
      printf("GAME OVER!\nYou encountered an obstacle greater than or equal to your length.\n");
      return 1;
    }
 
 for(i=1; i<length; i++){ //For hit your own body
  if(snake_body[0].x == snake_body[i].x && snake_body[0].y == snake_body[i].y){
     printf("GAME OVER!\nYou hit your own body.\n");
     return 1;
  }
 
 }
 
 return 0;

}

int move(snake_point* snake_body, int length, char *choice, Block*** board, int *dir_control) { //Moving function
    int i;
   if(length>1){
    if(snake_body[0].x<snake_body[1].x && *choice == 's'){
     printf("You can't turn the direction that goes through your own body!\n");
     *dir_control=1;
     return 0;
    }
    else if(snake_body[0].x>snake_body[1].x && *choice == 'w'){
     printf("You can't turn the direction that goes through your own body!\n");
     *dir_control=1;
     return 0;
    }
    else if(snake_body[0].y<snake_body[1].y && *choice == 'd'){
     printf("You can't turn the direction that goes through your own body!\n");
     *dir_control=1;
     return 0;
    }
    else if(snake_body[0].y>snake_body[1].y && *choice == 'a'){
     printf("You can't turn the direction that goes through your own body!\n");
     *dir_control=1;
     return 0;
    }
   }
    

    for (i = length - 1; i > 0; i--) {  //Translation movement
        snake_body[i].x = snake_body[i - 1].x;
        snake_body[i].y = snake_body[i - 1].y;
    }

    if (*choice == 'w') { //New coordinates of head
        snake_body[0].x = snake_body[0].x - 1;
    } else if (*choice == 's') {
        snake_body[0].x = snake_body[0].x + 1;
    } else if (*choice == 'd') {
        snake_body[0].y = snake_body[0].y + 1;
    } else if (*choice == 'a') {
        snake_body[0].y = snake_body[0].y - 1;
    }
    
    
    if (check_status(board,snake_body,length) == 1){
    return 0;
    }
    return 1;
}

snake_point* update(snake_point* snake_body, int* length, Block*** board, int* move_counter, int *dir_control) { //Update function
    char choice;
    int bax,bay,obx,oby,i,j;
    int obs_control=1,bait_control=1,body_control=1,control=1;
    int obstacle_counter=0;
    printf("Enter direction: ");
    scanf(" %c", &choice);
    (*move_counter)++; //Move counter for adding new obstacles or increasing obstacles' heights for every 5 move 
    
  if(move(snake_body, *length, &choice, board, dir_control)){ 
    if(board[snake_body[0].x][snake_body[0].y]->type == 'b'){ //If head encounters an obstacle
        board[snake_body[0].x][snake_body[0].y]->type = 'e';
        board[snake_body[0].x][snake_body[0].y]->value = '0';
        (*length)++;  //Increasing length
        
        snake_body = realloc(snake_body, sizeof(snake_point) * (*length)); //Realloc for new length
        if(choice == 'w'){ //Determining the coordinates of new part 
         snake_body[*length-1].x = snake_body[*length-2].x+1;
         snake_body[*length-1].y = snake_body[*length-2].y;
        }
        else if(choice == 's'){
         snake_body[*length-1].x = snake_body[*length-2].x-1;
         snake_body[*length-1].y = snake_body[*length-2].y;
        }
        else if(choice == 'd'){
         snake_body[*length-1].x = snake_body[*length-2].x;
         snake_body[*length-1].y = snake_body[*length-2].y-1;
        }
        else if(choice == 'a'){
         snake_body[*length-1].x = snake_body[*length-2].x;
         snake_body[*length-1].y = snake_body[*length-2].y+1;
        }
        srand(time(NULL));
        do{  //Generate new bait randomly
         bax = rand()%10;
         bay = rand()%10;
         control=1;
         obs_control=1;
         bait_control=1;
         if(board[bax][bay]->type != 'o'){
            obs_control=0;
         }
         
         for(i=0; i<=*length; i++){
           if(snake_body[i].x == bax && snake_body[i].y == bay){
              bait_control=0;
           }
         }
           if(bait_control != 0)
           control=0;
             printf("..\n");
        }while(obs_control || control);
        board[bax][bay]->type = 'b';
        board[bax][bay]->value = '0';
    }
    
    
    bait_control=1;
    control=1;
    for(i=0; i<10; i++){   //Counting obstacles
     for(j=0; j<10; j++){
      if(board[i][j]->type == 'o')
       obstacle_counter++;
     }
    }
    
     
    if(*move_counter % 5 == 0 && obstacle_counter<3){  //Adding obstacles for every 5 move until number of obstacle reachs 3
     srand(time(NULL));
        do{
         obx = rand()%10;
         oby = rand()%10;
         bait_control=1;
         body_control=1;
         control=1;
         if(board[obx][oby]->type != 'b'){
            bait_control=0;
         }
         
         for(i=0; i<*length; i++){
           if(snake_body[i].x == obx && snake_body[i].y == oby){
              body_control=0;
           }
         }
           if(body_control != 0)
           control=0;
          printf("..\n");
        }while(bait_control || control);
        if(board[obx][oby]->type == 'o'){
         board[obx][oby]->value = board[obx][oby]->value + rand()%9+1;
        }
        else{
        board[obx][oby]->type = 'o';
        board[obx][oby]->value = rand()%9+1;
        }
    }
    if(*move_counter % 5 == 0 && obstacle_counter == 3){  //If number of obstacle is 3 obstacle's height increasing by 1 every 5 moves
      for(i=0; i<10; i++){
       for(j=0; j<10; j++){
        if(board[i][j]->type == 'o' && board[i][j]->value < 9){
         board[i][j]->value = board[i][j]->value+1;
        }
       }
      }
    }
    
    
    if(board[snake_body[0].x][snake_body[0].y]->type == 'o' && board[snake_body[0].x][snake_body[0].y]->value < *length){//If snake encounters an obstacle that is smaller than it's length it breaks it
      board[snake_body[0].x][snake_body[0].y]->type = 'e';
      board[snake_body[0].x][snake_body[0].y]->value = '0';
    }
    
   } 
    return snake_body; 
   
}


void play(Block*** board) {
    int length = 1;
    int move_counter=0;
    int dir_control;
    snake_point* snake_body = (snake_point*)malloc(length * sizeof(snake_point));
    snake_body[0].x = 0;
    snake_body[0].y = 0;
        draw_board(board, snake_body, length);
    while (1) {
        
        
        snake_body = update(snake_body, &length, board, &move_counter, &dir_control);
        system("clear");
        if(check_status(board,snake_body,length) == 1)
        break;
        if(dir_control == 1){
        printf("You can't turn the direction that goes through your own body!\n");
        dir_control=0;
        }
        draw_board(board, snake_body, length);
        
    }

    free(snake_body);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            free(board[i][j]);
        }
        free(board[i]);
    }
    free(board);
}

int main() {
    Block*** board = init_board();
    play(board);
    return 0;
}
