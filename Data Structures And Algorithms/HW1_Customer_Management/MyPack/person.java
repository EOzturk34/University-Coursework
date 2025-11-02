package MyPack;

public class person{
    private String name = new String();
    private String surname = new String();
    private String address = new String();
    public String phone = new String();
    private int ID;
    //Setters
    public void setName(String name){
        this.name = name;
    }
    public void setSurname(String surname){
        this.surname = surname;
    }
    public void setAddress(String address){
        this.address = address;
    }
    public void setPhone(String phone){
        this.phone = phone;
    }
    public void setID(int ID){
        this.ID = ID;
    }
    //Getters
    public String getName(){
        return name; 
    }
    public String getSurname(){
        return surname;
    }
    public String getAddress(){
        return address;
    }
    public String getPhone(){
        return phone;
    }
    public int getID(){
        return ID;
    }
    
    
}
