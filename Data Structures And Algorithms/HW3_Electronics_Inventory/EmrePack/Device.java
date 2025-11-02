package EmrePack;
public interface Device {
    String getCategory();
    String getName();
    double getPrice();
    int getQuantity();

    void setCategory(String category);
    void setName(String name);
    void setPrice(double price);
    void setQuantity(int quantity);

    

    
}