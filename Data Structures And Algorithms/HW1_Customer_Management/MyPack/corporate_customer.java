package MyPack;

public class corporate_customer extends customer{
    private String company_name;
    //Setter and getter 
    public String getCompany_name(){
        return company_name;
    }

    public void SetCompany_name(String company_name){
         this.company_name = company_name;
    }

    @Override
    public void print_customer() { //Override print_customer method
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
        + getOperator_ID() + '\n'
        + "Company name: " 
        + company_name + '\n');
        print_orders();
    }
    
}