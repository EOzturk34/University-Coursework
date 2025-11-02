#include <stdio.h>
#define MAX_WORDS_NUMBER 1000
#define MAX_LENGTH_OF_WORD 50
#define SIZE 10000
#define SIZE2 1000

int main()
{
 int choice;
 int fileop_choice;
 FILE *fp;
 FILE *fp2;
 char product[500];
 int line_count;
 int line2=0;
 int menu_control=1;
 char holder,holder2;
 printf("Please enter your inputs in the formats given to you in the sections. Respects.\n");
 do
 {
    printf("Welcome operator, please select an option to continue:\n1-File Operations\n2-Query products\n3-Check stock status\n4-Stock control by brand\n5-Export report\nChoice:");
    scanf("%d",&choice);
    if(choice==1) //MENU 1
    { 
      do
      {
        printf("1-Add a new product\n2-Delete a product\n3-Update a product\n4-Add feature to a product\n5-Add a new stock entry\n6-Delete a stock entry\n7-Update a stock entry\n8-Back to menu\nYour choice: ");
        scanf("%d",&fileop_choice);
        
        
        if(fileop_choice==1) //ADD A PRODUCT
        { 
          char header[500];
          char pID_holder[50];
          line_count=0;
          fp=fopen("products.txt","r");
          while((holder=fgetc(fp)) != EOF){  // FUNCTION FOR AUTO-INCREMENTED pID
            if(holder == '\n') 
            line_count++;
          }
          fclose(fp);
          fp=fopen("products.txt","r");
          fscanf(fp,"%[^,],%s",pID_holder,header);
          fclose(fp);
          fp=fopen("products.txt","a");
          printf("Enter your product and it's features(FOR EXAMPLE:d,kola,pepsi,25 (or more with same pattern)):\n");
          printf("%s\n",header);
          scanf("%s",product);
          fprintf(fp,"%d,%s\n",line_count,product);
          fclose(fp);
        }
        
        
        else if(fileop_choice==2) //DELETE A PRODUCT
        {
        int choice;
        int a=1;
        char product_file_name[]="products.txt";
        char temp[]="temp.txt";                    // TEMPORARY FILE FOR DELETE
        fp=fopen("products.txt","r");
        fp2=fopen("temp.txt","w");
        if(fp2 == NULL) //IF FILE DOES NOT EXİST CREATE FILE 
        {
        fp2 = fopen("temp.txt", "wb");
        }
        int count=0;
        printf("Enter your pID for delete: ");
        scanf("%d",&choice);
           
        while((holder=fgetc(fp)) != EOF) {  //FUNCTION FOR COPYING FIRST LINE(HEADER LINE)
        fprintf(fp2,"%c",holder);  
        if(holder == '\n') line2++;
        if(line2 == 1) break; 
        }
         
        while(!feof(fp))
	{
        count++;
  	int line=0;               
     	while((holder=fgetc(fp)) != EOF) {  //FUNCTION FOR JUMPING THE pID's  
        if(holder == ',') line++;
        if(line == 1) break; 
     	}
    	if(feof(fp)) break;
    	if(count != choice)
    	{
     	fprintf(fp2,"%d,",a);
     	a++;
    	}
  	while((holder=getc(fp)) && (holder != '\n' ))
  	{
   
     	if(count != choice)    //IF COUNT = CHOICED LINE FOR DELETE DON'T WRITE IT TO TEMP FILE
    	fprintf(fp2,"%c",holder);
        }
  	if(count != choice)
  	fprintf(fp2,"\n");
        }
	fclose(fp);
	fclose(fp2);
	printf("%d",count);
	remove(product_file_name);
	rename(temp,product_file_name);

       }
       
       
       else if(fileop_choice == 3) //UPDATE A PRODUCT
       { 
       
         FILE *fp,*fp2;
        int num_comma;
        char line[1000];
        char word[200];
        char new_word[200];
        char holder;
        int count=0,word_found=0;
        int update_pID;
        int comma_in_line=0,comma_count=0;
        int i;
        int end_line_control=0;
        int end_file_control=0;
        printf("Enter pID of product for change: ");   // UPDATING INFORMATIONS FROM USER
        scanf("%d",&update_pID);
        printf("Enter the feature you want to change ((PLEASE DO NOT USE ANY COMMA OR NEW LINE CHARACTER!!!) FOR EXAMPLE: brand): ");
        scanf("%s", word);
        printf("Enter new value of feature ((PLEASE DO NOT USE ANY COMMA OR NEW LINE CHARACTER!!!) FOR EXAMPLE: samsung): ");
        scanf("%s",new_word);
        fp = fopen("products.txt", "r");
         while (fgets(line, 1000, fp) != NULL) {  // COUNTING COMMA NUMBER BEFORE SELECTED FEATURE 
        for (i = 0; line[i] != '\0'; i++) {
            if (line[i] == ',') {
                count++;
            }
            if (line[i] == word[0]) {
             int j;
             for (j = 1; word[j] != '\0'; j++) {
              if (line[i + j] != word[j]) {
               break;
              }
             }
             if (word[j] == '\0') {
              word_found = 1;
               break;
             }
            }
          }
          if (word_found) {
            break;
        }
    }
    if (word_found==0) {
         printf("'%s' not found in file.\n", word);
    } 
    fclose(fp);
    fp = fopen("products.txt", "r");
    while((holder=fgetc(fp)) != '\n'){  //COUNTING COMMA NUMBERS IN A LINE
    if(holder==',')
    comma_in_line++;
    } 
    fclose(fp);
    
    
    
 char product_file_name[]="products.txt"; //CREATE TEMPORARY FILE
 char temp_file_name[]="temp.txt";
 fp=fopen("products.txt","r");
 fp2=fopen("temp.txt","w");
 if(fp2 == NULL){
  fp2=fopen("temp.txt","wb");
 }
 while(!end_file_control){

 while((holder=fgetc(fp)) != EOF){
 if(holder==',')
 comma_count++;
 if(comma_count != update_pID*comma_in_line+count)   //FUNCTION CALCULATED HOW MANY COMMAS ARE THERE BEFORE SELECTED PRODUCT'S SELECTED FEATURE AND IT REACH THAT LOCATION WRITE NEW VAULE OF FEATURE
 fprintf(fp2,"%c",holder);
 else{
 while ((holder = fgetc(fp)) != ',' && holder != '\n'){
 }
 if(count!=comma_in_line){
 fprintf(fp2,",%s,",new_word);
 comma_count++;
 }
 else{
 fprintf(fp2,",%s\n",new_word);
 comma_count++;
 }
 }
}
 if(holder == EOF){
 end_file_control=1;
 remove(product_file_name);
 rename(temp_file_name,product_file_name);
 break;
 }
 fclose(fp);
 fclose(fp2);
 
 
 }
 
 
 }
 	
       //ADD A FEATURE
       else if(fileop_choice==4){
 char holder;
 FILE *fp,*fp2;
 int count=1;
 int end_file_control;
 int choice_line_for_add;
 
 char new_feature[50];
 printf("Please give your new feature name (PLEASE DO NOT USE COMMA(,) OR NEW LİNE CHARACTER JUST ENTER NEW FEATURE NAME!!!!!!!!): ");
 scanf("%s",new_feature);
 char product_file_name[]="products.txt"; //CREATE TEMPORARY FILE
 char temp_file_name[]="temp.txt";
 fp=fopen("products.txt","r");
 fp2=fopen("temp.txt","w");
 if(fp2 == NULL){
   fp2=fopen("temp.txt","wb");
 }
 
 while((holder=fgetc(fp)) != '\n'){  //FUNCTOIN THAT WRITES FIRST LINE AND NEW FEATURE TO END OF LINE 
 fprintf(fp2,"%c",holder);
 }
 fprintf(fp2,",%s",new_feature);
 fprintf(fp2,"\n");
 
 while(!end_file_control){

 while((holder=fgetc(fp)) != EOF){
 
 if(holder == '\n')                  //WRITING none END OF LINES
 fprintf(fp2,",none");
 
 
 fprintf(fp2,"%c",holder);
 if(holder == '\n')
 count++;
}
 if(holder == EOF){
 end_file_control=1;
 remove(product_file_name);
 rename(temp_file_name,product_file_name);
 break;
 }
 fclose(fp);
 fclose(fp2);
 
 
 }

       }
       
      //ADD A STOCK
      else if(fileop_choice==5){
          int pID_for_stocks;
          int current_stock;
          char branch[500];
          char rest_of_line[500];
          line_count=0;
          fp=fopen("stocks.txt","r");
          while((holder=fgetc(fp)) != EOF){
            if(holder == '\n') 
            line_count++;
          }
          fclose(fp);
          fp=fopen("stocks.txt","a");
          printf("Please enter pID of product: ");
          scanf("%d",&pID_for_stocks);
          printf("Please enter branch of product: ");
          scanf("%s",branch);
          printf("Please enter current stock of product: ");
          scanf("%d",&current_stock);
          fprintf(fp,"%d,%d,%s,%d\n",line_count,pID_for_stocks,branch,current_stock);
          fclose(fp);
          }
     //DELETE A STOCK
     else if(fileop_choice==6){
        FILE *fp,*fp2;
        int choice;        //CREATE TEMPORARY FILE 
        char holder;
        int a=1;
        int line2=0;
        char product_file_name[]="stocks.txt";
        char temp[]="temp.txt";
        fp=fopen("stocks.txt","r");
        fp2=fopen("temp.txt","w");
        if(fp2 == NULL) //if file does not exist, create it
        {
        fp2 = fopen("temp.txt", "wb");
        }
        int count=0;
        printf("Enter your sID for delete: ");
        scanf("%d",&choice);
           
        while((holder=fgetc(fp)) != EOF) {  //WRITING FIRST LINE TO TEMP FILE
        fprintf(fp2,"%c",holder);  
        if(holder == '\n') line2++;
        if(line2 == 1) break; 
        }
         
        while(!feof(fp))
	{
        count++;
  	int line=0;               
     	while((holder=fgetc(fp)) != EOF) {    //JUMPING sID's
        if(holder == ',') line++;
        if(line == 1) break; 
     	}
    	if(feof(fp)) break;
    	if(count != choice)
    	{                                  //WRITING sID's TO TEMP FILE FROM 1 
     	fprintf(fp2,"%d,",a);
     	a++;
    	}
  	while((holder=getc(fp)) && (holder != '\n' ))
  	{
   
     	if(count != choice)                   //IF COUNT = CHOICED LINE FOR DELETE DON'T WRITE IT TO TEMP FILE
    	fprintf(fp2,"%c",holder);
        }
  	if(count != choice)
  	fprintf(fp2,"\n");
        }
	fclose(fp);
	fclose(fp2);
	
	remove(product_file_name);
	rename(temp,product_file_name);
     
     
     }
     //UPDATE A STOCK
     else if(fileop_choice==7){
      FILE *fp,*fp2;             
        int num_comma;
        char line[1000];
        char word[200];
        char new_word[200];
        char holder;
        int count=0,word_found=0;
        int update_pID;
        int comma_in_line=0,comma_count=0;
        int i;
        int end_line_control=0;
        int end_file_control=0;                           //INFORMATIONS FOR UPDATING
        printf("Enter sID of product for change: ");
        scanf("%d",&update_pID);
        printf("Enter the feature you want to change ((PLEASE DO NOT USE ANY COMMA OR NEW LINE CHARACTER!!!) FOR EXAMPLE: branch): ");
        scanf("%s", word);
        printf("Enter new value of feature ((PLEASE DO NOT USE ANY COMMA OR NEW LINE CHARACTER!!!) FOR EXAMPLE: Kocaeli): ");
        scanf("%s",new_word);
        fp = fopen("stocks.txt", "r");
         while (fgets(line, 1000, fp) != NULL) {  // COUNTING COMMA NUMBER BEFORE SELECTED FEATURE 
        for (i = 0; line[i] != '\0'; i++) {
            if (line[i] == ',') {
                count++;
            }
            if (line[i] == word[0]) {
             int j;
             for (j = 1; word[j] != '\0'; j++) {
              if (line[i + j] != word[j]) {
               break;
              }
             }
             if (word[j] == '\0') {
              word_found = 1;
               break;
             }
            }
          }
          if (word_found) {
            break;
        }
    }
    if (word_found==0) {
        printf("'%s' not found in file.\n", word);
    } 
    fclose(fp);
    fp = fopen("stocks.txt", "r");
    while((holder=fgetc(fp)) != '\n'){          //COUNTING COMMA NUMBERS IN A LINE
    if(holder==',')
    comma_in_line++;
    } 
    fclose(fp);
    
    
    
 char product_file_name[]="stocks.txt";   //CREATING TEMPORARY FILE
 char temp_file_name[]="temp.txt";
 fp=fopen("stocks.txt","r");
 fp2=fopen("temp.txt","w");
 if(fp2 == NULL){
  fp2=fopen("temp.txt","wb");
 }
 while(!end_file_control){

 while((holder=fgetc(fp)) != EOF){  //FUNCTION CALCULATED HOW MANY COMMAS ARE THERE BEFORE SELECTED PRODUCT'S SELECTED FEATURE AND IT REACH THAT LOCATION WRITE NEW VAULE OF FEATURE
 if(holder==',')
 comma_count++;
 if(comma_count != update_pID*comma_in_line+count)
 fprintf(fp2,"%c",holder);
 else{
 while ((holder = fgetc(fp)) != ',' && holder != '\n'){  
 }
 if(count!=comma_in_line){
 fprintf(fp2,",%s,",new_word);
 comma_count++;
 }
 else{
 fprintf(fp2,",%s\n",new_word);
 comma_count++;
 }
 }
}
 if(holder == EOF){
 end_file_control=1;
 remove(product_file_name);
 rename(temp_file_name,product_file_name);
 break;
 }
 fclose(fp);
 fclose(fp2);
 
 
 }
     
     }
      }while(fileop_choice!=8);
    }
    
    
    //MENU 2 SEARCH
    else if(choice==2){
 FILE *fp;
 char holder;
 int line_status[SIZE];
 int line_index=0;
 int i,x,y,j,b,price_control=0,filter_control2=0;
 int num_comma=0;
 int word_found_index=0;
 int line_counter=0;
 char line[SIZE];
 char line2[SIZE];
 char word_found[50][50];
 char hold_filtred_products_array[500][500];
 int filter_control=0,filter_counter=0,filter_choice;
 float price_min,price_max;
 
 while(!filter_control)
{
     //ENTERING FILTER
 printf("Enter 1 for adding filter\nEnter 2 for entering price filter\nEnter 3 for stop entering filter\nChoice: ");
 scanf("%d",&filter_choice);
 if(filter_choice == 1){
 printf("Please don't bother typing the name features, just enter the word you want to be filtered.\nEXAMPLE: Samsung\nPLEASE DO NOT USE A COMMA OR NEWLINE CHARACTER!!!!\n");
 printf("Enter %d. filter: ",filter_counter+1);
 scanf("%s",word_found[filter_counter]);
 filter_counter++;
 filter_control2=1;
 x=1;
 }
 else if(filter_choice == 2){
 printf("Enter minumum price: ");
 scanf("%f",&price_min);
 printf("Enter maximum price: ");
 scanf("%f",&price_max);
 price_control=1;
 y=1;
 }
 else if(filter_choice == 3)                                       
 filter_control=1;
 if(x+y==2)
  filter_control=1;                                  
}
 
 fp=fopen("products.txt","r");       //FINDING COMMA NUMBERS IN A LINE FOR DETERMINING INSIDE OF FSCANF
 while((holder=fgetc(fp)) !='\n'){
  if(holder==',')
  num_comma++;
 }
 fclose(fp);
if(filter_control2){
 fp=fopen("products.txt","r");
 while(fgets(line,SIZE,fp) != NULL)
 {                                         //THE MAIN FUNCTION OF THIS PART. CHECKING WHICH LINES THE WORDS FROM USER IS IN. AND ACCORDING TO THIS MAKE line_status ARRAY'S ELEMENTS 1 OR 0  
  while(word_found_index <filter_counter) 
  {
   for(b=0; word_found[word_found_index][b] != '\0'; b++)
    {
   
    }
    
   for(i=0; line[i] != '\0'; i++)
   {  
       
     if(line [i] == word_found[word_found_index][0] && line [i-1] == ',' && (line [i+b] == ',' || line [i+b] == '\n'))
      {
         for(j=1; word_found[word_found_index][j] != '\0'; j++)
         {
           if(line [i+j] != word_found[word_found_index][j] )
           break;
         }
         if(word_found[word_found_index][j] == '\0')
         { 
           line_status[line_index]=1;
           break;
         }
         else
         line_status[line_index]=0;      
     }
     else
      line_status[line_index]=0;
   }
   word_found_index++;
   if(line_status[line_index]==0)
   break;
  }
   word_found_index=0;
   line_index++;
 }

 fclose(fp);
 
if(price_control == 0){
 line_status[0]=0;
 line_index=0;
 fp=fopen("products.txt","r");
 while(fgets(line2,SIZE,fp) != NULL)          
 { 
   
   if(line_status[line_index] == 1)
   {
      for(i=0; line2[i]!='\0'; i++){
         hold_filtred_products_array[j][i]=line2[i];   ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED PRODUCTS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
        }
        hold_filtred_products_array[j][i]='\0';
        printf("%s\n",hold_filtred_products_array[j]); ///////////////!!!!!!!!!!!!!!!!!! PRINTING FILTRED PRODUCTS FROM A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
        j++;                                                                        
      line_counter++;
      line_index++;
   }
   else
   line_index++;
 }
 fclose(fp);
}
}
if(price_control){      //PRICE FILTER PART

    fp=fopen("products.txt","r");
    int pID;
    char type;
    char name[SIZE];
    char brand[SIZE];
    float price;
    char rest_of_line[SIZE];
    int r=1;
    int price_status_array[SIZE];
    
    

    
    fp = fopen("products.txt", "r");
    if (fp == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    int found = 0;  
    
    fscanf(fp, "%*[^\n]");  /// ASSIGNMENT SUPPRESSION FOR FIRST LINE

    if(num_comma != 4)    //DETERMINING INSIDE OF F SCANF ACCORDING TO COMMA NUMBERS 
  {
    while (fscanf(fp, "%d,%c,%[^,],%[^,],%f,%[^\n]", &pID, &type, name, brand, &price, rest_of_line) == 6) {
        if (price_min<=price && price<=price_max){
            price_status_array[r]=1;                      //DETERMINING price_status_array's ELEMENTS AS 1 OR 0 ACCORDING TO FILTER 
      }
      else{
      price_status_array[r]=0;
      }
      r++;
    }                              
  }
  else
  {
    while (fscanf(fp, "%d,%c,%[^,],%[^,],%f", &pID, &type, name, brand, &price) == 5) {
       if (price_min<=price && price<=price_max) {
            price_status_array[r]=1;
      }
      else{
      price_status_array[r]=0;
      }
      r++;
    }
  }

 fclose(fp);
if(filter_control2){
 price_status_array[0]=0;
 line_status[0]=0;
 line_index=0;
 fp=fopen("products.txt","r");
 while(fgets(line2,SIZE,fp) != NULL)  
 { 
   line_status[line_index]=line_status[line_index]*price_status_array[line_index];
   if(line_status[line_index] == 1)                      
   {
       for(i=0; line2[i]!='\0'; i++){
         hold_filtred_products_array[j][i]=line2[i];    ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED PRODUCTS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////     
        }
        hold_filtred_products_array[j][i]='\0';
        printf("%s\n",hold_filtred_products_array[j]);  ///////////////!!!!!!!!!!!!!!!!!! PRINTING FILTRED PRODUCTS FROM A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
        j++;                                                                        
      line_counter++;
      line_index++;
   }
   else
   line_index++;
 }
 fclose(fp);
}
else if(!filter_control2){
 price_status_array[0]=0;
 r=0;
 fp=fopen("products.txt","r");
 while(fgets(line2,SIZE,fp) != NULL)  
 { 
   
   if(price_status_array[r] == 1)
   {
       for(i=0; line2[i]!='\0'; i++){
         hold_filtred_products_array[j][i]=line2[i];    ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED PRODUCTS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
        }
        hold_filtred_products_array[j][i]='\0';
        printf("%s\n",hold_filtred_products_array[j]); ///////////////!!!!!!!!!!!!!!!!!! PRINTING FILTRED PRODUCTS FROM A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
        j++;                                                                        
      line_counter++;
      r++;
   }
   else
   r++;
 }
 fclose(fp);
}
}
    
    }
   // MENU 3 CHECK OF STOCK STATUS
   else if(choice ==3){
   int check_stock_choice;
   int word_counter;
   int line_counter=0;
   char check_stock_array[50][50];
   char hold_filtred_stocks_array[1000][1000];
   char line[SIZE];
   char line2[SIZE];
   int line_status[SIZE];
   int line_index=0;
   int word_found_index=0;
   int b;
   int i,j;
   // SELECTION OF OPERATION 
   printf("1-Query the stock of a given product in a specified branch\n2-List the stock of all products in a specified branch\n3-List the out-of-stock products in a specified branch\n4-Exit\nEnter your stock check choice(1,2,3):");
   scanf("%d",&check_stock_choice);
   if(check_stock_choice==1){
   printf("Please enter your product ID: ");
   scanf("%s",check_stock_array[0]);
   printf("Please enter your branch: ");
   scanf("%s",check_stock_array[1]);
   word_counter=2;
   }
   else if(check_stock_choice==2){
   printf("Please enter your branch: ");
   scanf("%s",check_stock_array[0]);
   word_counter=1;
   }
   else if(check_stock_choice==3){
   printf("Please enter your branch: ");
   scanf("%s",check_stock_array[0]);
   word_counter=1;
   }
 
  
if(check_stock_choice !=4){  
   fp=fopen("stocks.txt","r");
 while(fgets(line,SIZE,fp) != NULL) //THE MAIN FUNCTION OF THIS PART. CHECKING WHICH LINES THE WORDS FROM USER IS IN. AND ACCORDING TO THIS MAKE line_status ARRAY'S ELEMENTS 1 OR 0 
 {           
  while(word_found_index <word_counter)
  {
   for(b=0; check_stock_array[word_found_index][b] != '\0'; b++)
    {
                                                                 
    }
    
   for(i=0; line[i] != '\0'; i++)
   {  
       
     if(line [i] == check_stock_array[word_found_index][0] && line [i-1] == ',' && (line [i+b] == ',' || line [i+b] == '\n'))
      {
         for(j=1; check_stock_array[word_found_index][j] != '\0'; j++)
         {
           if(line [i+j] != check_stock_array[word_found_index][j] )
           break;
         }
         if(check_stock_array[word_found_index][j] == '\0')
         { 
           line_status[line_index]=1;
           break;
         }
         else
         line_status[line_index]=0;      
     }
     else
      line_status[line_index]=0;
   }
   word_found_index++;
   if(line_status[line_index]==0)
   break;
  }
   word_found_index=0;
   line_index++;
 }

 fclose(fp);
 fp=fopen("stocks.txt","r");
 line_status[0]=0;
 line_index=0;
 if(check_stock_choice == 1 || check_stock_choice == 2){
 while(fgets(line2,SIZE,fp) != NULL)
 { 
  
   if(line_status[line_index] == 1)
   {
      for(i=0; line2[i]!='\0'; i++){
         hold_filtred_stocks_array[j][i]=line2[i];    ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED STOCKS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
        }
        hold_filtred_stocks_array[j][i]='\0';
        printf("%s\n",hold_filtred_stocks_array[j]);  ///////////////!!!!!!!!!!!!!!!!!! PRINTING FILTRED STOCKS FROM A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
        j++;                                                                        
      line_counter++;
      line_index++;
   }
   else
   line_index++;
  
 }
  
}
 else if(check_stock_choice == 3){
 while(fgets(line2,SIZE,fp) != NULL)
 { 
   
   if(line_status[line_index] == 0)
   {
      for(i=0; line2[i]!='\0'; i++){
         hold_filtred_stocks_array[j][i]=line2[i];   ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED STOCKS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
        }
        hold_filtred_stocks_array[j][i]='\0';
        printf("%s\n",hold_filtred_stocks_array[j]);  ///////////////!!!!!!!!!!!!!!!!!! PRINTING FILTRED STOCKS FROM A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
        j++;                                                                        
      line_counter++;
      line_index++;
   }
   else
   line_index++;
 }
 }
 fclose(fp);

 }  
   
   }
  // MENU 4 BRAND STOCK CONTROL
  else if(choice ==4){
  FILE *fp,*fp2;
  char brand_stock_word[1][50];
  char brand_stock_control_array[SIZE][100];
  char line[SIZE];
  char pID[100],type[100],name[100],brand[100],price[100],rest_of_line[500],sID[100],pID_from_stock[100],branch[100],current_stock[100];
  int line_status[1000];
  int line_index=0;
  int b,n=0;
  int num_comma=0;
  int i,j,k;
  int product_counter=0;
  fp=fopen("products.txt","r");       //FINDING COMMA NUMBERS IN A LINE FOR DETERMINING INSIDE OF FSCANF
 while((holder=fgetc(fp)) !='\n'){
  if(holder==',')
  num_comma++;
 }
 fclose(fp);
 
  
  printf("Please enter your brand(FOR EXAMPLE: pepsi): ");
  scanf("%s",brand_stock_word[0]);
 for(b=0; brand_stock_word[0][b] != '\0'; b++)
    {
   
    }
 fp=fopen("products.txt","r");
 while(fgets(line,SIZE,fp) != NULL)   //THE MAIN FUNCTION OF THIS PART. CHECKING WHICH LINES THE WORDS FROM USER IS IN. AND ACCORDING TO THIS MAKE line_status ARRAY'S ELEMENTS 1 OR 0 
 {
   for(i=0; line[i] != '\0'; i++)
   {  
       
     if(line [i] == brand_stock_word[0][0] && line [i-1] == ',' && (line [i+b] == ',' || line [i+b] == '\n'))
      {
         for(j=1; brand_stock_word[0][j] != '\0'; j++)
         {
           if(line [i+j] != brand_stock_word[0][j] )
           break;
         }
         if(brand_stock_word[0][j] == '\0')
         { 
           line_status[line_index]=1;
           break;
         }
         else
         line_status[line_index]=0;      
     }
     
   }
   line_index++;
 }
 
 fclose(fp);
 
 
 fp=fopen("products.txt","r");
 fp2=fopen("stocks.txt","r");   
 line_index=0;
if(num_comma!=4){
 while(fscanf(fp,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",pID,type,name,brand,price,rest_of_line) == 6 && fscanf(fp2,"%[^,],%[^,],%[^,],%[^\n]",sID,pID_from_stock,branch,current_stock)==4){
 
   if(line_status[line_index] == 1){
    for(b=n; b<n+1; b++){
     for(j=0; pID[j]!='\0'; j++)
      brand_stock_control_array[b][j]=pID[j];   ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED STOCKS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
     brand_stock_control_array[b][j]='\0';
     
     
     for(i=0; price[i]!='\0'; i++)  
      brand_stock_control_array[b+1][i]=price[i];   ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED STOCKS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
     brand_stock_control_array[b+1][i]='\0';
     
     
     for(k=0; current_stock[k]!='\0'; k++)
      brand_stock_control_array[b+2][k]=current_stock[k];   ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED STOCKS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
     brand_stock_control_array[b+2][k]='\0';
     
     product_counter++;
    }
    n=n+3;
    
   }
   line_index++;
 }
 
}
else if(num_comma==4){
while(fscanf(fp,"%[^,],%[^,],%[^,],%[^,],%[^\n]",pID,type,name,brand,price) == 5 && fscanf(fp2,"%[^,],%[^,],%[^,],%[^\n]",sID,pID_from_stock,branch,current_stock)==4){
 
   if(line_status[line_index] == 1){
    for(b=n; b<n+1; b++){
     for(j=0; pID[j]!='\0'; j++)
      brand_stock_control_array[b][j]=pID[j];   ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED STOCKS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
     brand_stock_control_array[b][j]='\0';
     
     
     for(i=0; price[i]!='\0'; i++)  
      brand_stock_control_array[b+1][i]=price[i];   ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED STOCKS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
     brand_stock_control_array[b+1][i]='\0';
     
     
     for(k=0; current_stock[k]!='\0'; k++)
      brand_stock_control_array[b+2][k]=current_stock[k];   ///////////////!!!!!!!!!!!!!!!!!! STORING FILTRED STOCKS IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
     brand_stock_control_array[b+2][k]='\0';
     
     product_counter++;
    }
    n=n+3;
    
   }
   line_index++;
 }

}
 fclose(fp);
 fclose(fp2);
 
 for(b=0; b<product_counter*3; b++){              ///////////////!!!!!!!!!!!!!!!!!! PRINTING FILTRED STOCKS FROM A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
 printf("%s",brand_stock_control_array[b]);
 if(b%3==2)
 printf("\n");
 if(b%3==0 || b%3==1)
 printf(",");
 }
 printf("\n");
 
 
  }
  // MENU 5 STOCK REPORT
  else if(choice==5){
  int g,i,k,j=0,max_pID=0;
  char sID[100],branch[100];
  float current_stock;
  int pID_from_stock;
  float medyan,min,max;
  int count_array[]={0};
  fp=fopen("products.txt","r"); 
  while((holder=fgetc(fp)) != EOF){
   if(holder=='\n')
    max_pID++;
  }
  fclose(fp);
  
  float hold_stocks_array[][20]={0};
  float min_max_medyan_array[max_pID][20];
  for(i=1; i<max_pID; i++){
   fp2=fopen("stocks.txt","r");
   fscanf(fp2,"%*[^\n]");
   while(fscanf(fp2,"%[^,],%d,%[^,],%f",sID,&pID_from_stock,branch,&current_stock) == 4){
   
    if(pID_from_stock == i){
     hold_stocks_array[i][j] = current_stock;       //STORING EVERY STOCKS OF EACH PRODUCT IN A ARRAY FOR BUBBLE SORT
     j++;
     count_array[i]++;
    }
   }
   fclose(fp2);
   j=0;
   
   }
 
fp2=fopen("stockreport.txt","w");
fprintf(fp2,"pID,Min Stock,Max Stock,Median Stock\n");
for(k=1; k<max_pID-1; k++){                               // BUBBLE SORT
 for(i=0; i<20; i++){
  for(j=1; j<20-i; j++){
   if(hold_stocks_array[k][j-1] > hold_stocks_array[k][j]){
    float temp = hold_stocks_array[k][j];
    hold_stocks_array[k][j] = hold_stocks_array[k][j-1];
    hold_stocks_array[k][j-1] = temp; 
   }
  }
 }
 min=hold_stocks_array[k][20-count_array[k]];    // CALCULATION MIN,MAX,MEDYAN
 max=hold_stocks_array[k][19];
 if(count_array[k]%2==1){
  medyan=hold_stocks_array[k][(19+20-count_array[k])/2];
 }
 else{
  medyan=(float)(hold_stocks_array[k][(19+20-count_array[k])/2] + hold_stocks_array[k][(19+20-count_array[k])/2+1])/2;
 }
 int a=0;
 min_max_medyan_array[k][a]=min;        ///////////////!!!!!!!!!!!!!!!!!! STORING MIN,MAX,MEDYAN IN A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
 min_max_medyan_array[k][a+1]=max;
 min_max_medyan_array[k][a+2]=medyan;
                                    ///////////////!!!!!!!!!!!!!!!!!! PRINTING MIN,MAX,MEDYAN FROM A 2D ARRAY AS YOU WANT !!!!!!!!!!!!!!!!!!!!!!!////////////////
 fprintf(fp2,"%d,%.2f,%.2f,%.2f\n",k,min_max_medyan_array[k][a],min_max_medyan_array[k][a+1],min_max_medyan_array[k][a+2]);  
 
 a=a+3;
 
}
 fclose(fp2);
menu_control=0;
 }
    
 
 }while(menu_control);

}
