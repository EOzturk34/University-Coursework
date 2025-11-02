package MyPack;

public class order{
    private String product_name;
    private int count;
    private int total_price;
    private int status;
    private int customer_ID;
    //Getters
    public String getProduct_name(){
        return product_name;
    }
    public int getCount(){
        return count;
    }
    public int getTotal_price(){
        return total_price;
    }
    public int getStatus(){
        return status;
    }
    public int getCustomer_ID(){
        return customer_ID;
    }
    //Setters
    public void setProduct_name(String product_name){
        this.product_name = product_name;
    }
    public void setCount(int count){
        this.count = count;
    }
    public void setTotal_price(int total_price){
        this.total_price = total_price;
    }
    public void setStatus(int status){
        this.status = status;
    }
    public void setCustomer_ID(int customer_ID){
        this.customer_ID = customer_ID;
    }
    
    //Method to print information of the order
    public void print_order() {
        System.out.print("Product name: "
        + product_name  
        + " - Count: "
        + count
        + " - Total price: "
        + total_price
        + " - Status: ");
        if(status == 0)
            System.out.println("Initialized");
        else if(status == 1)
            System.out.println("Processing");
        else if(status == 2)
            System.out.println("Completed");
        else if(status == 3)
            System.out.println("Cancelled");
    }
    
}