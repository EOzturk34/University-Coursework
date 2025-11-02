import MyPack.*;
import java.util.Scanner;

public class Main{
    public static void main(String[] args) {
        int flag = 0;
        ReadFile file = new ReadFile("content.txt");
        file.readFile();
        file.callDefineFunctions();
        Scanner scanner = new Scanner(System.in);
        System.out.println("Please enter your ID...");
        int userID;

        if (scanner.hasNextInt()) {
            userID = scanner.nextInt();
            for(int i = 0; file.getOperatorArray()[i] != null; ++i){
                if(userID == file.getOperatorArray()[i].getID()){
                    System.out.println("*** Operator Screen ***");
                    file.getOperatorArray()[i].print_operator(); 
                    flag = 1;
                    break;  
                }
             }
             for(int i = 0; file.getRetail_CustomerArray()[i] != null; ++i){
                if(userID == file.getRetail_CustomerArray()[i].getID()){
                    System.out.println("*** Customer Screen ***");
                    file.getRetail_CustomerArray()[i].print_customer();
                    flag = 1; 
                    break;   
                }
             }
             for(int i = 0; file.getCorporate_CustomerArray()[i] != null; ++i){
                if(userID == file.getCorporate_CustomerArray()[i].getID()){
                    System.out.println("*** Customer Screen ***");
                    file.getCorporate_CustomerArray()[i].print_customer();
                    flag = 1;
                    break;    
                }
             }
             if(flag == 0)
                System.out.println("No operator/customer was found with ID " + userID + ". Please try again.");
            
            scanner.close();
       }    
       else {
          System.out.println("Invalid input. Please enter a valid integer.");
          scanner.close();
          System.exit(1); 
        }


    }
    
}
 