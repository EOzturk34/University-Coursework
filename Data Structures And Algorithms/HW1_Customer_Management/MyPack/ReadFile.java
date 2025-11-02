package MyPack;
import java.io.File;
import java.util.Scanner;

public class ReadFile {
    private String filePath;
    //Arrays for objects
    private retail_customer[] retail_CustomerArray = new retail_customer[100];
    private corporate_customer[] corporate_CustomerArray = new corporate_customer[100];
    private operator[] operatorArray = new operator[100];
    private order[] orderArray = new order[100];
    private String[] IDArray = new String[200]; //Array for ID
    //Getter Functions
    public retail_customer[] getRetail_CustomerArray() { return retail_CustomerArray; }
    public corporate_customer[] getCorporate_CustomerArray() { return corporate_CustomerArray; }
    public operator[] getOperatorArray() { return operatorArray; }
    public order[] getOrderArray() { return orderArray; }
    //Consturctor
    public ReadFile(String filePath){
        this.filePath = filePath;
    }
    //Method to count semicolons in each line
    private int countSemicolon(String line){
        int count = 0;
        for(int i = 0; i<line.length(); ++i){
            if(line.charAt(i) == ';')
                count++;
            
        }  
        return count;  
    }
    //Method to check intgers are positive or negative
    private int isPositive(int index, int lastIndex, String[] piecedLine, char type){
        int number;
        for(int i = index; i < lastIndex; ++i){
            number = Integer.parseInt(piecedLine[i]);
            if(type == 'o'){
                if(i == 4 && number < 0)
                    return 0;
                else if(i != 4 && number <= 0)
                    return 0;
            }
            else{
                if(number <= 0)
                    return 0;
            }
        }
       return 1;
    }
    //Method to check integers are greater than int max value or not
    private int isLargerThanIntMaxValue(String[] piecedLine, int index, int lastIndex) {
        String intMaxValueStr = String.valueOf(Integer.MAX_VALUE);
        for(int i = index; i < lastIndex; ++i){
            int lengthDifference = piecedLine[i].length() - intMaxValueStr.length();
            if (lengthDifference > 0) {
                return 1;
            } 
            else if (lengthDifference == 0) {
                for (int j = 0; j < piecedLine[i].length(); ++j) {
                    if (piecedLine[i].charAt(j) < intMaxValueStr.charAt(j)) {
                        return 0;
                    } 
                    else if (piecedLine[i].charAt(j) > intMaxValueStr.charAt(j)) {
                        return 1;
                    }
                }
                return 0;
            }
        }
        return 0;
    }
    //Method to check integers are convertible or not
    private int isConvertible(int index, int lastIndex, String[] piecedLine){
        for(int i = index; i<lastIndex; ++i){
            for(int j = 0; j<piecedLine[i].length(); ++j){
                if(piecedLine[i].charAt(j) < 48 || piecedLine[i].charAt(j) > 57)
                    return 0;    
            }
        }
        return 1;
    }
    //Method to check ID of the object is already used or not
    private int isExistingID(String[] piecedLine){
        int flag = 0;
        for(int i = 0; IDArray[i] != null; ++i){
        if(IDArray[i].equals(piecedLine[5]))
            flag = 1;
        }
        return flag;
    }
    //Method to read file with error handling
    public void readFile() {
        try {
            File file = new File(filePath);
            Scanner scanner = new Scanner(file);
            
            while (scanner.hasNextLine()) {
                int flag = 1;
                String currentLine = scanner.nextLine();
                String[] piecedLine = currentLine.split(";");
                for(int i = 0; i < piecedLine.length; ++i){ //Check if any of elements of the line is empty
                    if(piecedLine[i] == "")
                        flag = 0;
                }
                if(flag == 0){
                   //ignore line
                }
                else if("order".equals(piecedLine[0])){ // If type is order this block will be run
                    if(countSemicolon(currentLine) != 5){  //Check semicolon count
                        //ignore line
                    }
                    else if(piecedLine.length != 6){  //Check counts of elements
                        //ignore line
                    }
                    else if(isLargerThanIntMaxValue(piecedLine, 2, 6) == 1){ //Check length of integers
                        //ignore line
                    }
                    else if(isConvertible(2, 6, piecedLine) == 0){ //Check intgers are convertible or not
                        //ignore line
                    }
                    else if(isPositive(2, 6, piecedLine, 'o') == 0){ //Check intgers are positive or not
                        //ignore line
                    }
                    else if(piecedLine[4].charAt(0) > 51){ //Check status is greater than 3 or not
                        //ignore line
                    }
                    else{ //If there is no problem with the line create a new object and add it to array
                        int j;
                        for(j = 0; orderArray[j] != null; ++j){
                        }
                        order newOrder = new order();
                        newOrder.setProduct_name(piecedLine[1]);
                        int count = Integer.parseInt(piecedLine[2]);
                        int total_price = Integer.parseInt(piecedLine[3]);
                        int status = Integer.parseInt(piecedLine[4]);
                        int costumer_ID = Integer.parseInt(piecedLine[5]);
                        newOrder.setCount(count);
                        newOrder.setTotal_price(total_price);
                        newOrder.setStatus(status);
                        newOrder.setCustomer_ID(costumer_ID);
                        orderArray[j] = newOrder;
                    }
                }
                else if("retail_customer".equals(piecedLine[0])){ //If type is retail_customer this block will be run
                    if(countSemicolon(currentLine) != 6){  //Check semicolon count
                        //ignore line
                    }
                    else if(piecedLine.length != 7){  //Check counts of elements
                        //ignore line
                    }
                    else if(isLargerThanIntMaxValue(piecedLine, 5, 7) == 1){  //Check length of integers
                        //ignore line
                    }
                    else if(isConvertible(5, 7, piecedLine) == 0){  //Check intgers are convertible or not
                        //ignore line
                    }
                    else if(isPositive(5, 7, piecedLine, 'o') == 0){ //Check intgers are positive or not
                        //ignore line 
                    }
                    else if(isExistingID(piecedLine) == 1){ //Check if the ID already exist 
                      //ignore line   
                    }
                    else if(piecedLine[5].equals(piecedLine[6])){ //Check if the customerID and operatorID are equal 
                        //ignore line 
                    }
                    else{  //If there is no problem with the line create a new object and add it to array
                        int i,j;
                        for(i = 0; IDArray[i] != null; ++i){
                        }
                        IDArray[i] = piecedLine[5];
                        for(j = 0; retail_CustomerArray[j] != null; ++j){
                        }
                        retail_customer newRetail_customer = new retail_customer();
                        newRetail_customer.setName(piecedLine[1]);
                        newRetail_customer.setSurname(piecedLine[2]);
                        newRetail_customer.setAddress(piecedLine[3]);
                        newRetail_customer.setPhone(piecedLine[4]);
                        int ID = Integer.parseInt(piecedLine[5]);
                        newRetail_customer.setID(ID);
                        int operator_ID = Integer.parseInt(piecedLine[6]);
                        newRetail_customer.setOperator_ID(operator_ID);
                        retail_CustomerArray[j] = newRetail_customer;;
                    }
                    
                }
                else if("corporate_customer".equals(piecedLine[0])){  
                    if(countSemicolon(currentLine) != 7){   //Check semicolon count
                        //ignore line
                    }
                    else if(piecedLine.length != 8){  //Check counts of elements
                        //ignore line
                    }
                    else if(isLargerThanIntMaxValue(piecedLine, 5, 7) == 1){ //Check length of integers
                        //ignore line
                    }
                    else if(isConvertible(5, 7, piecedLine) == 0){  //Check intgers are convertible or not
                        //ignore line
                    }
                    else if(isPositive(5, 7, piecedLine, 'o') == 0){ //Check intgers are positive or not
                        //ignore line 
                    }
                    else if(isExistingID(piecedLine) == 1){ //Check if the ID already exist 
                      //ignore line;   
                    }
                    else if(piecedLine[5].equals(piecedLine[6])){ //Check if the customerID and operatorID are equal
                        //ignore line 
                    }
                    else{  //If there is no problem with the line create a new object and add it to array
                        int i,j;
                        for(i = 0; IDArray[i] != null; ++i){
                        }
                        IDArray[i] = piecedLine[5];
                        for(j = 0; corporate_CustomerArray[j] != null; ++j){
                        }
                        corporate_customer newCorporate_customer = new corporate_customer();
                        newCorporate_customer.setName(piecedLine[1]);
                        newCorporate_customer.setSurname(piecedLine[2]);
                        newCorporate_customer.setAddress(piecedLine[3]);
                        newCorporate_customer.setPhone(piecedLine[4]);
                        int ID = Integer.parseInt(piecedLine[5]);
                        newCorporate_customer.setID(ID);
                        int operator_ID = Integer.parseInt(piecedLine[6]);
                        newCorporate_customer.setOperator_ID(operator_ID);
                        newCorporate_customer.SetCompany_name(piecedLine[7]);
                        corporate_CustomerArray[j] = newCorporate_customer;
                    }

                }
                else if("operator".equals(piecedLine[0])){
                    if(countSemicolon(currentLine) != 6){  //Check semicolon count
                        //ignore line
                    }
                    else if(piecedLine.length != 7){  //Check counts of elements
                        //ignore line
                    }
                    else if(isLargerThanIntMaxValue(piecedLine, 5, 6) == 1){  //Check length of integers
                        //ignore line
                    }
                    else if(isConvertible(5, 6, piecedLine) == 0){  //Check intgers are convertible or not
                        //ignore line
                    }
                    else if(isPositive(5, 6, piecedLine, 'o') == 0){ //Check intgers are positive or not
                        //ignore line
                    }
                    else if(isExistingID(piecedLine) == 1){
                      //ignore line  
                    }
                    else{  //If there is no problem with the line create a new object and add it to array
                        int i,j;
                        for(i = 0; IDArray[i] != null; ++i){
                        }
                        IDArray[i] = piecedLine[5];
                        for(j = 0; operatorArray[j] != null; ++j){
                        }
                        operator newOperator = new operator();
                        newOperator.setName(piecedLine[1]);
                        newOperator.setSurname(piecedLine[2]);
                        newOperator.setAddress(piecedLine[3]);
                        newOperator.setPhone(piecedLine[4]);
                        int ID = Integer.parseInt(piecedLine[5]);
                        newOperator.setID(ID);
                        int wage = Integer.parseInt(piecedLine[6]);
                        newOperator.setWage(wage);
                        operatorArray[j] = newOperator;
                    }

                }
                else{
                    //ignore line;
                }
                
                
            }
            scanner.close();
        } catch (Exception e) {
            System.out.println("Dosya okunurken bir hata oluştu: " + e.getMessage());
        }
    }
    //Method to call define_orders and define_customers methods of customer objects and operator objects
    public void callDefineFunctions(){
        //Call all operators from the array and define customers for each of them
        for(int i = 0; operatorArray[i] != null; ++i){
            operatorArray[i].define_customers(corporate_CustomerArray);
            operatorArray[i].define_customers(retail_CustomerArray);
        }
        //Call all corporate customers from the array and define orders for each of them
        for(int i = 0; corporate_CustomerArray[i] != null; ++i){
            corporate_CustomerArray[i].define_orders(orderArray);
        }
        //Call all retail customers from the array and define orders for each of them
        for(int i = 0; retail_CustomerArray[i] != null; ++i){
            retail_CustomerArray[i].define_orders(orderArray);
        }
    }

}
