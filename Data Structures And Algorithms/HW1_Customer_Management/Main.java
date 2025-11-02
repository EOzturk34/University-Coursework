import MyPack.*;
import java.util.Scanner;

public class Main{
    public static void main(String[] args) {
        int flag = 0;
        ReadFile file = new ReadFile("content.txt"); //Call constructors for file object
        file.readFile(); //Read file fill the arrays
        file.callDefineFunctions(); //Call define_orders and define_customers methods
        Scanner scanner = new Scanner(System.in);
        System.out.println("Please enter your ID...");
        int userID;

        if (scanner.hasNextInt()) {
            userID = scanner.nextInt();
            if(userID < 0){ //If ID is negative warn the user
                System.out.println("ID cannot be negative."); 
                return;
            }
            for(int i = 0; file.getOperatorArray()[i] != null; ++i){ //Check the operators for userID
                if(userID == file.getOperatorArray()[i].getID()){  //If userID matches call print function of the operator
                    System.out.println("*** Operator Screen ***");
                    file.getOperatorArray()[i].print_operator(); 
                    flag = 1;
                    break;  
                }
             }
             for(int i = 0; file.getRetail_CustomerArray()[i] != null; ++i){ //Check the retail customers for userID
                if(userID == file.getRetail_CustomerArray()[i].getID()){ //If userID matches call print function of the customer
                    System.out.println("*** Customer Screen ***");
                    file.getRetail_CustomerArray()[i].print_customer();
                    flag = 1; 
                    break;   
                }
             }
             for(int i = 0; file.getCorporate_CustomerArray()[i] != null; ++i){ //Check the corporate customers for userID
                if(userID == file.getCorporate_CustomerArray()[i].getID()){ //If userID matches call print function of the customer
                    System.out.println("*** Customer Screen ***");
                    file.getCorporate_CustomerArray()[i].print_customer();
                    flag = 1;
                    break;    
                }
             }
             if(flag == 0) //If userID doesn't match with any of the customers or operators warn the user
                System.out.println("No operator/customer was found with ID " + userID + ". Please try again.");
            
            scanner.close();
       }    
       else { //If user enters an invalid input warn the user
          System.out.println("Invalid input. Please enter a valid integer.");
          scanner.close();
          System.exit(1); 
        }


    }
    
}
 