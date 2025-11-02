package MyPack;

public class customer extends person{
    private order[] orders = new order[100];
    private int operator_ID;
    //Method to print customer's information
    public void print_customer() {
        System.out.print("Name & Surname: " 
        + getName() + ' '
        + getSurname() + '\n'
        + "Address: "
        + getAddress() + '\n'
        + "Phone: "
        + getPhone() + '\n'
        + "ID: "
        + getID() + '\n'
        + "Operator ID: "
        + operator_ID + '\n');
        print_orders();
    }
    //Method to print customer's orders' information 
    public void print_orders() {
        for(int i = 0; orders[i] != null; ++i){
            System.out.print("Order #" + (i+1) + " => ");
            orders[i].print_order();
            
        }
        
    }
    //Method to fill orders array
    public void define_orders(order[] orders) {
        int i,j;
        for(i = 0; orders[i] != null; ++i){
            if(orders[i].getCustomer_ID() == this.getID()){
                for(j = 0; this.orders[j] != null; ++j){    
                }
                this.orders[j] = orders[i];
            }
            
        }
    }
    //Setter and getter
    public void setOperator_ID(int operator_ID){
        this.operator_ID = operator_ID;
    }

    public int getOperator_ID(){
        return operator_ID;
    }
    

}