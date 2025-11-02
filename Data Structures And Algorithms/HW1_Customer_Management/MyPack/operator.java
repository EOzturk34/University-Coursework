package MyPack;

public class operator extends person{
    private int wage;
    private customer[] customers = new customer[100];
    //Method to print the operator's information
    public void print_operator() {
        System.out.print("----------------------------" + '\n'
        + "Name & Surname: " 
        + getName() + ' '
        + getSurname() + '\n'
        + "Address: "
        + getAddress() + '\n'
        + "Phone: "
        + getPhone() + '\n'
        + "ID: "
        + getID() + '\n'
        + "Wage: "
        + wage + '\n'
        + "----------------------------" + '\n');
        
        if(customers[0] == null) //If operator doesn't have any customer warn the user
            System.out.println("This operator doesn't have any customer." + '\n' + "----------------------------");
       
       
       print_customers(); //Call print_customers method for print customers' information    
    
    }
    
    public void print_customers() {
        for(int i = 0; customers[i] != null; ++i){
            System.out.println("Customer Screen");
            customers[i].print_customer(); //Call print_customer method for print the customers' information    
            System.out.println("----------------------------");
        }
    }
    //Method for fill customers array
    public void define_customers(customer[] customers) {
        int i,j;
        for(i = 0; customers[i] != null; ++i){
            if(customers[i].getOperator_ID() == this.getID()){
                for(j = 0; this.customers[j] != null; ++j){    
                }
                this.customers[j] = customers[i];
            }
        }
        
    }
    //Setters and getters
    public void setWage(int wage){
        this.wage = wage;
    }

    public int getWage(){
        return wage;
    }
    
    
}