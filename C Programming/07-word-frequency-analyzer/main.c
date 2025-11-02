#include <stdio.h>
#include <string.h>
#include <math.h>
#define MAX_WORD_SIZE 12
#define MAX_WORD_IGNORE 1000
#define MAX_WORD_INPUT 300000
#define MAX_WORD_DICTIONARY 2000
#define MAX_LENGTH 1200
#define MAX_WORDS 100

void remove_hidden_characters(char* str) {  //A function for remove extra hidden characters
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n' || str[i] == '\r' ) {
            str[i] = '\0';
            
            break;
        }
        i++;
     }
}
// Read input function
int read_text(const char * text_file, const char * ignore_file, char words[][MAX_WORD_SIZE]){
 FILE *fp,*fp2;
 char holder;
 char ignore_words[MAX_WORD_IGNORE][MAX_WORD_SIZE];
 int control=1,i=0,j=0,k=0,m,letter_counter=1,ignore_control=1;
 fp=fopen(text_file,"r");
 if(fp == NULL){    //If there is a problem retruns -1
  return -1;
 }
 
 fp2=fopen(ignore_file,"r");
 
 if(fp2 == NULL){
  return -1;
 }
 while(control){
  while((holder=fgetc(fp2)) != '\n'){  //Function that reads ignored words
   if(holder == EOF)
   control=0;
   if(holder == EOF)
   break;
   else{
   ignore_words[k][j] = holder;
   j++;
  }
 }
 ignore_words[k][j]='\0';
 j=0;
 k++;
}
fclose(fp2);


for(m=0; m<k-1; m++){
remove_hidden_characters(ignore_words[m]);  //Remove hidden chracters
}

j=0;
control=1;

 while(control){
  ignore_control=1;
  while(1){           //Main function for read input without ignored words
   holder=fgetc(fp);
   if(holder == EOF)
   control=0;
   if(holder == EOF || holder == '\n' || (holder >=32 && holder <= 47) || (holder >= 58 && holder <= 64) || (holder >= 91 && holder <= 96) || (holder >= 123 && holder <= 126)){
   break;
   }
   else{
   words[i][j] = holder;
   j++;
   letter_counter++;
   }
  }
  words[i][j] = '\0';
  letter_counter=0;
  for(int g=0; g<k-1; g++){
   if(ignore_control && strcmp(ignore_words[g],words[i]) == 0){   //If any index is filled with ignored words this index is used again in this way ignored word will be deleted
   i--;
   }
  }
  
  while(1){
   holder=fgetc(fp); //If there are repeatedly separator operators this function ensure that reading will finish at the right place
   if(holder == EOF){
   control=0;
   break;
   }
   if(holder == EOF || holder == '\n' || (holder >=32 && holder <= 47) || (holder >= 58 && holder <= 64) || (holder >= 91 && holder <= 96) || (holder >= 123 && holder <= 126)){
   continue;
   }
   else{
   i++;
   j=0;
   words[i][j] = holder;
   j++;
   break;
   }
   
  }
  
 
}
  i++;
  j=0;
  words[i][j]='-'; //Adding sentinel element to input array
  j++;
  words[i][j]='\0';
  j=0;
  
  
  return (i);//Returns number of words in input
     
}
//Read dictionary array
 int read_dict(const char * file_name, char dict[][MAX_WORD_SIZE]){

 char holder;
 int vector_counter = 0,control=1,i=0,j=0,g,k,l;
 float vector_array[MAX_WORD_DICTIONARY][300];
 float vector;
 FILE *fp;
 fp=fopen(file_name,"r");
 if(fp == NULL){   //If there is a problem returns -1
  return -1;
 }
 while((holder=fgetc(fp))!= '\n'){
  
 }
 
 while((holder=fgetc(fp))!= ' '){
  
 }
 
 while((holder=fgetc(fp))){  //Counting number of vectors for a word
  if((holder >= 65 && holder <= 90) || (holder >= 97 && holder <= 122))
  break;
  if(holder == '.')
  vector_counter++;
  
 }
 
 fclose(fp);
 fp=fopen(file_name,"r");
 if(fp == NULL){
  return -1;
 }
 
while((holder=fgetc(fp))!= '\n'){  //Jumping first line
  
 } 
 
while(control){
  while( (holder=fgetc(fp)) != ' '){ //Reading the dictionary
   if(holder == EOF){
    control=0;
    break;
   }
   if(holder != '\n'){
   dict[i][j] = holder;
   j++;
   }
  }
  dict[i][j] = '\0';
  i++;
  j=0;
  
  for(g=0; g<vector_counter; g++){
   fscanf(fp,"%f",&vector);
   vector_array[k][g] = vector;
   
  }
  k++;
  
 }
  
  i--;
  j=0;
  dict[i][j]='-';
  j++;
  dict[i][j]='\0';
  j=0;
 
 return (i); //Returns number of words in dictionary
}
// Dissimilarity function
double dissimilarity(char *w1, char w2[], char dict[][MAX_WORD_SIZE], float threshould, int *threshould_control){
 char word[50];
 int i,index=0;
 for(i=0; w1[i]!='\0'; i++)  //Taking user's word
 word[i] = w1[i];
 word[i] ='\0';
 
 i=0;
 while(1){
  if(strcmp(word,dict[i]) == 0) //Finding user's word index int dictionary array for jumping this array in calculation part in order to escape 0 distance
  break;
  i++;
  index++;
}

 FILE *fp;
 char holder;
 int g,k=0,control=1, dictionary_counter=0,vector_counter=0;
 float vector;
 float vector_array[MAX_WORD_DICTIONARY][300];
 double sum=0;
 double distance_array[MAX_WORD_DICTIONARY];
  fp=fopen("dictionary.txt","r");
 while((holder=fgetc(fp))!= '\n'){  //Jumping first line
  
 }
 
 while((holder=fgetc(fp))!= ' '){ //Jumping first word
  
 }
 
 while((holder=fgetc(fp))){      //Counting number of vectors for a word
  if((holder >= 65 && holder <= 90) || (holder >= 97 && holder <= 122))
  break;
  if(holder == '.')
  vector_counter++;
  
 }
 
 fclose(fp);
 fp=fopen("dictionary.txt","r");
 for(g=0; dict[g][0] != '-'; g++) 
 dictionary_counter++;
 fclose(fp);
 
 fp=fopen("dictionary.txt","r");
 while((holder=fgetc(fp))!= '\n'){ 
  
 }
 
while(control){ //Holding words
  while((holder=fgetc(fp)) != ' '){
   if(holder == EOF){
    control=0;
    break;
   }
  }
  if(holder == EOF)
  break;
  for(g=0; g<200; g++){  //Reading vectors for each word
   fscanf(fp,"%f",&vector);
   vector_array[k][g] = vector;
   
  }
  k++;
  
 }
fclose(fp);

for(g=0; g<dictionary_counter; g++){  //Calculating closest word
 
  for(k=0; k<vector_counter; k++){
  sum = sum + (vector_array[index][k] - vector_array[g][k]) * (vector_array[index][k] - vector_array[g][k]);
  }
  sum=sqrt(sum);
  distance_array[g] = 1/sum;
  sum=0;
  
 }
 double maks;
 int a;
 maks=distance_array[0];
 
 for(i=1; i<dictionary_counter; i++){
 if(i != index){
  if(distance_array[i]>maks){
  maks=distance_array[i];
  a=i;
  }
 }
 }
 
 
 if(maks>threshould)
 *threshould_control=1;
 else
 *threshould_control=0;

 for(i=0; dict[a][i]!='\0'; i++)  //Completing output argument
 w2[i] = dict[a][i];
 w2[i]='\0';
 
 return (maks); //Returns inverse of dissimilarity 
}
//Histogram function
int histogram(const char words[][MAX_WORD_SIZE], const int occurrences[], char hist[][MAX_WORD_SIZE+5+20]){
 int m,i;
 int maks;
 maks=occurrences[0];
 for(i=1; occurrences[i]!=-1; i++){ //Finding scale
  if(occurrences[i] > maks)
  maks=occurrences[i];
 
 }
 
 int scale=1;
 while(maks/scale>20){
  scale++;
 }
 printf("scale: %d\n",scale);
 
 for(m=0; occurrences[m]!=-1; m++){ //Writing histogram
  if(occurrences[m] == 0){
   printf("%-25s  NO MATCHES\n",words[m]);
  }
  else{
   printf("%-25s  %-25s\n",words[m],hist[m]);
   
  }
 }
 
 return (scale); //Returns scale
}

int main()
{
 FILE *fp;
 const char *text_file = "input.txt"; 
 const char *ignore_file = "ignore.txt";
 const char *file_name= "dictionary.txt";
 int i,j,m,f,u,exact_counter=0,dictionary_control=1,threshould_control=0,closest_counter=0;
 int word_status[100];
 int occurrences[100];
 char words[MAX_WORD_INPUT][MAX_WORD_SIZE];
 char dict[MAX_WORD_DICTIONARY][MAX_WORD_SIZE];
 char hist[100][MAX_WORD_SIZE+5+20];
 char user_words[100][MAX_WORD_SIZE];
 char w2[50];
 read_text(text_file, ignore_file, words);
 read_dict(file_name, dict);
 int index=0;
 float threshould = 0.15;
 char sentence[MAX_LENGTH];
 char separate_words[MAX_WORDS][MAX_LENGTH];

 printf("Please enter your words with space (MAKSIMUM 11 CHARACTER AND ONE FOR NULL CHARACTER) FOR EXAMPLE:Gebze Gtu Computer: ");
 fgets(sentence, sizeof(sentence), stdin);

 int wordCount = 0;
 int wordIndex = 0;         //Separating the user's input word by word 

 char* token = strtok(sentence, " ");
 while (token != NULL && wordCount < MAX_WORDS) {
 int i = 0;
 while (token[i] != '\0' && token[i] != '\n' && i < MAX_LENGTH - 1) {
  separate_words[wordCount][i] = token[i];
  i++;
 }
 separate_words[wordCount][i] = '\0';

 wordCount++;
 token = strtok(NULL, " ");
 }
 
for (i = 0; i < wordCount; i++) {
  
 }
 
if(wordCount>1){  //If user enter more than one word this functions will be used
for(m=0; m<i; m++){     
 for(j=0; words[j][0]!='-'; j++){ //Checking if word is in input.txt and if it is counting
  if(strcmp(words[j],separate_words[m]) == 0)
  exact_counter++;
  
 }

if(exact_counter == 0){       //Checking if word is in dictionary.txt and if it is finding the index
 for(j=0; dict[j][0]!='-'; j++){
  if(strcmp(dict[j],separate_words[m]) == 0){
  dictionary_control=0;
  break;
  }
  index++;
 }
 
 }
 
 if(exact_counter != 0){          //If word is in input take the word to user_words array and incrementing occurrences
  
  for(f=0; separate_words[m][f]!='\0'; f++){
  user_words[m][f]=separate_words[m][f];
  }
  user_words[m][f]='\0';
  for(u=0; u<exact_counter; u++){
   word_status[m] = 0;
   occurrences[m]++;
  }
   
 }
 
 if(exact_counter == 0 && dictionary_control){ //If the word is not in both the input and the dictionary, the actions to be taken
  for(f=0; separate_words[m][f]!='\0'; f++){
  user_words[m][f]=separate_words[m][f];
  }
  user_words[m][f]='\0';
  occurrences[m]=0;
  
 }
 
 if(exact_counter == 0 && !dictionary_control){  //If the word is not in the input but exists in the dictionary, actions to be taken
   
    
    dissimilarity(separate_words[m], w2, dict, threshould, &threshould_control);
    
    
    for(f=0; separate_words[m][f]!='\0'; f++){
    user_words[m][f]=w2[f];
    }
     user_words[m][f]='\0';
    
    
    if(threshould_control){
      for(j=0; words[j][0]!='-'; j++){
       if(strcmp(words[j],w2) == 0)
        closest_counter++;
      }
      occurrences[m]=closest_counter;
      word_status[m] = 1;
    
    }
    else{
    occurrences[m]=0;
    
    }
    
  }
  
 exact_counter=0;
 closest_counter=0;
 dictionary_control=1;
}
 user_words[m][0]='-';
 user_words[m][1]='\0';
 occurrences[m]=-1;
 int maks;
 maks=occurrences[0];
 for(i=1; i<wordCount; i++){
  if(occurrences[i] > maks)
  maks=occurrences[i];
 
 }
 
 int scale=1;
 while(maks/scale>20){
  scale++;
 }
 
 for(m=0; m<wordCount; m++){  //Filling hist array according to occurrences and scale
 if(occurrences[m]==0){
  hist[m][0] = '0';
  hist[m][1] = '\0';
 }
  else if(occurrences[m] != 0 && word_status[m] == 0){
   for(u=0; u<occurrences[m]/scale; u++){
   hist[m][u]='*';
   
  }
  hist[m][u]='\0';
   
  }
  else if(occurrences[m] != 0 && word_status[m] == 1){
   for(u=0; u<occurrences[m]/scale; u++){
   hist[m][u]='+';
   
  }
  hist[m][u]='\0';
  
  
  }
 }
 histogram(user_words,occurrences, hist);
 
}

else if (wordCount==1){  //If user enter only one word actions to be taken
exact_counter=0;
closest_counter=0;
dictionary_control=1;
for(m=0; m<1; m++){

 for(j=0; words[j][0]!='-'; j++){ //Checking if the word is in input or not
  if(strcmp(words[j],separate_words[m]) == 0)
  exact_counter++;
  
 }

if(exact_counter == 0){   //Checking if the word is in dictionary or not
 for(j=0; dict[j][0]!='-'; j++){
  if(strcmp(dict[j],separate_words[m]) == 0){
  dictionary_control=0;
  break;
  }
  index++;
 }
 
 }
 
 if(exact_counter != 0){  
  
  printf("“%s” appears in “input.txt” %d times.\n",separate_words[m],exact_counter);
  
 }
 if(exact_counter == 0 && dictionary_control){  //If word is not in input but is in dictionary actions to be taken
  printf("%s   NO MATCHES\n",separate_words[m]);
 }
 
 
 if(exact_counter == 0 && dictionary_control == 0){
   dissimilarity(separate_words[m], w2, dict, threshould, &threshould_control);
    if(threshould_control){
      for(j=0; words[j][0]!='-'; j++){
       if(strcmp(words[j],w2) == 0)
        closest_counter++;
      }
      if(closest_counter == 0)
      printf("%s   NO MATCHES\n",separate_words[m]);
      else
      printf("“%s” doesn’t appear in “input.txt” but “%s” appears %d times.\n",separate_words[m], w2, closest_counter);
     }
     else
     printf("%s   NO MATCHES\n",separate_words[m]);
    
  }
 
 exact_counter=0;
 closest_counter=0;
 
  }
 
 }

 

} 

