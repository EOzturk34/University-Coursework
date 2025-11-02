package EmrePack;
public class Laptop implements Device{
    private String category;
    private String name;
    private double price;
    private int quantity;
    /**
     * Constructor has O(n) complexity
     * It takes 4 parameter and initiliazes relevant fields
     */
    public Laptop(String category, String name, double price, int quantity){
        this.category = category;
        this.name = name;
        this.price = price;
        this.quantity = quantity;
    }
    /**
     * All getters has O(1) complexity
     * They are just returning relevant fields
     */
    @Override
    public String getCategory(){ return category; }
    @Override
    public String getName(){ return name; }
    @Override
    public double getPrice(){ return price; }
    @Override
    public int getQuantity(){ return quantity; }
    /**
     * All setters has O(1) complexity
     * They are just setting relevant fields
     */
    @Override
    public void setCategory(String category){ this.category = category; }
    @Override
    public void setName(String name){ this.name = name; }
    @Override
    public void setPrice(double price){ this.price = price; }
    @Override
    public void setQuantity(int quantity){ this.quantity = quantity; }
}
