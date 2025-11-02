package EmrePack;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.Scanner;

import java.util.Iterator;

public class Inventory {
    private LinkedList<ArrayList<Device>> devicesLists = new LinkedList<>();
    private Scanner scanner;

    /**
    * Constructor for the Inventory class.
    * This constructor initializes 5 empty ArrayLists for the device list and starts a Scanner object.
    * <p>
    * Complexity: O(1)
    */
    public Inventory(){
        for(int i = 0; i < 5; ++i){
            devicesLists.add(new ArrayList<>());
        }
        scanner = new Scanner(System.in);
    }
    /**
    * Lists all devices in the inventory.
    * Complexity: O(n), where n is the total number of devices across all lists.
    * This method iterates through all device lists and then through each device,
    * making it linearly dependent on the total number of devices.
    */
    public void listAllDevices(){
        System.out.println("Device List:");
        for(int i = 0; i < devicesLists.size(); ++i){
            for(Device device : devicesLists.get(i)){
                System.out.println((i+1) + ". Category: " + " Name: " + device.getName() + ", Price: " + device.getPrice() + "$, Quantity: " + device.getQuantity());
            }
        }

    }
    /**
    * Finds the index of the category provided.
    * Complexity: O(1), since the switch-case structure executes a fixed number of cases
    * and is independent of the input size.
    */
    private int findIndex(String category){
        switch(category){
            case "Smart Phone":
                return 0;
            case "Smart Watch":
                return 1;
            case "Computer":
                return 2;
            case "Laptop":
                return 3;
            case "TV":
                return 4;
            default:
                return -1;
        }
    }
    /**
    * Creates a device based on the provided category, name, price, and quantity.
    * Complexity: O(1), as it uses a switch-case structure with a fixed number of cases,
    * which does not depend on the size of the input.
    */
    private Device createDevice(String category, String name, double price, int quantity){
        switch(category){
            case "Smart Phone":
                return new SmartPhone(category, name, price, quantity);
            case "Smart Watch":
                return new SmartWatch(category, name, price, quantity);
            case "Computer":
                return new Computer(category, name, price, quantity);
            case "Laptop":
                return new Laptop(category, name, price, quantity);
            case "TV":
                return new TV(category, name, price, quantity);
            default:
                return null;
        }
    }
    /**
    * Adds a new device to the inventory based on the provided details.
    * Complexity: O(1), as it involves fixed operations: creating a device, finding an index,
    * and adding to an ArrayList, all of which have constant time complexity.
    */
    public void addNewDevice(String category, String name, double price, int quantity){
        Device newDevice = createDevice(category, name, price, quantity);
        if(newDevice == null){
            System.out.println("There is no category named " + category + ".");
            return;
        }
        int index = findIndex(category);
        if(index != -1){
            devicesLists.get(index).add(newDevice);
            System.out.println(  newDevice.getCategory() + ", "  
                               + newDevice.getName() + ", "
                               + newDevice.getPrice() + ", " 
                               + newDevice.getQuantity()
                               + " amount added...");
        }
    }
    /**
    * Removes a device with the specified name from the inventory.
    * Complexity: O(n), where n is the total number of devices across all lists.
    * It potentially iterates through all devices to find and remove the specified one,
    * making the complexity linearly dependent on the total number of devices.
    */

    public void removeDevice(String deviceName){
        boolean flag = false;
        for(ArrayList<Device> devices :  devicesLists){
            Iterator<Device> iterator = devices.iterator();
            while(iterator.hasNext()){
                Device currentDevice = iterator.next();
                if(currentDevice.getName().equals(deviceName)){
                    flag = true;
                    iterator.remove();
                    System.out.println(  currentDevice.getCategory() + ", "
                                       + currentDevice.getName() + ", "
                                       + currentDevice.getPrice() + ", "
                                       + currentDevice.getQuantity()
                                       + " amount removed...");
                }
            }
        }
        if(!flag)
            System.out.println("There is no device named " + deviceName + ".");
    }
    /**
    * Updates a device with the specified name from the inventory.
    * Complexity: O(n), where n is the total number of devices across all lists.
    * It potentially iterates through all devices to find and update the specified one,
    * making the complexity linearly dependent on the total number of devices.
    */
   public void updateDeviceDetails(String currentName){
        boolean flag = false;
        String newName;
        String newPrice;
        String newQuantity;
        System.out.print(" Enter new name (leave blank to keep current name): ");
        newName = scanner.nextLine();
        System.out.print(" Enter new price (leave blank to keep current price): ");
        newPrice = scanner.nextLine();
        System.out.println(" Enter new quantity (leave blank to keep current quantity): ");
        newQuantity = scanner.nextLine();
        for(ArrayList<Device> deviceList : devicesLists){
            for(Device device : deviceList){
                if(device.getName().equals(currentName)){
                    flag = true;
                    if(!newName.isEmpty() || !newName.isBlank()){
                        device.setName(newName);
                    }
                    if(!newPrice.isEmpty() || !newPrice.isBlank()){
                        double doubleNewPrice = Double.parseDouble(newPrice);
                        device.setPrice(doubleNewPrice);
                    }
                    if(!newQuantity.isEmpty() || !newQuantity.isBlank()){
                        int intNewQuantity = Integer.parseInt(newQuantity);
                        device.setQuantity(intNewQuantity);
                    }
                }
            }
        
        }
        if(!flag)
            System.out.println("There is no device named " + currentName + ".");
        
    }
    /**
    * Finds cheapest device from the inventory.
    * Complexity: O(n), where n is the total number of devices across all lists.
    * It potentially iterates through all devices to find cheapest one,
    * making the complexity linearly dependent on the total number of devices.
    */
    public void findCheapestDevice() {
        Device cheapestDevice = null;
        double cheapestPrice = Double.MAX_VALUE; 
        for (ArrayList<Device> deviceList : devicesLists) {
            for (Device device : deviceList) {
                if (device.getPrice() < cheapestPrice) {
                    cheapestDevice = device;
                    cheapestPrice = device.getPrice();
                }
            }
        }
        System.out.println( "The cheapest device is:" + '\n' 
                          + "Category: " + cheapestDevice.getCategory() + ", "
                          + "Name: " + cheapestDevice.getName() + ", "
                          + "Price: " + cheapestDevice.getPrice() + ", "
                          + "Quantity: " + cheapestDevice.getQuantity());
    }
    /**
    * Sorts all of the devices.
    * Complexity: O(n) for combining, O(log(n)) for sorting, 
    * where n is the total number of devices across all lists.
    * Since O(n) will grow much faster than O(log(n)) complexity is O(n)
    * It potentially iterates through all devices to find cheapest one,
    * making the complexity linearly dependent on the total number of devices.
    */
    public void sortDevices(){
        ArrayList<Device> allLists = new ArrayList<>();
        for(ArrayList<Device> deviceList: devicesLists){
            allLists.addAll(deviceList);
        }
        allLists.sort(Comparator.comparingDouble(Device :: getPrice));
        System.out.println("Device sorted by price:");
        for(Device device : allLists){
            System.out.println( "Category: " + device.getCategory()
                               + ", Name: " + device.getName() 
                               + ", Price: " + device.getPrice()
                               + ", Quantity: " + device.getQuantity());
        }
    }
    /**
    * Calculates total value of prices.
    * Complexity: O(n), where n is the total number of devices across all lists.
    * It potentially iterates through all devices to calculate total price,
    * making the complexity linearly dependent on the total number of devices.
    */
    public double calculateTotalValue(){
        double total = 0;
        for(ArrayList<Device> deviceList : devicesLists){
            for(Device device : deviceList){
                total += device.getPrice();
            }

        }
        System.out.println("Total inventory value: $" + total);
        return total;
    }
    /**
    * Restock a device with the specified name from the inventory.
    * Complexity: O(n), where n is the total number of devices across all lists.
    * It potentially iterates through all devices to find devices with specified name,
    * making the complexity linearly dependent on the total number of devices.
    */
    public void restockDevice(String name){
        System.out.print("Do you want to add or remove stock? (Add/Remove): ");
        String choice = scanner.nextLine();
        int change;
        for(ArrayList<Device> deviceList : devicesLists){
            for(Device device : deviceList){
                if(name.equals(device.getName())){
                    switch(choice){
                        case "Add":
                            System.out.print("Enter the quantity to add: ");
                            change = scanner.nextInt();
                            scanner.nextLine();
                            device.setQuantity(device.getQuantity() + change);
                            System.out.println(device.getName() + " restocked. New quantity: " + device.getQuantity());
                        break;
                        case "Remove":
                            System.out.print("Enter the quantity to remove: ");
                            change = scanner.nextInt();
                            scanner.nextLine();
                            device.setQuantity(device.getQuantity() - change);
                            System.out.println(device.getName() + " restocked. New quantity: " + device.getQuantity());
                        break;
                        default:
                            System.out.println("Invalid input!");
                        break;
                    }
                }
            }
        }
        
        
      
        

    }
    /**
    * Exports the report of inventory.
    * Complexity: O(n), where n is the total number of devices across all lists.
    * It potentially iterates through all devices to print,
    * it calls calculateTotalValue which is O(n)
    * but iteration and calling calculateTotalValue are not nested
    * so making the complexity linearly dependent on the total number of devices.
    */
    public void exportInventoryReport(String fileName) {
        try {
            PrintWriter writer = new PrintWriter(fileName);
            writer.println("Electronics Shop Inventory Report");
            writer.println("Generated on: " + java.time.LocalDate.now()); // Bugünün tarihi ile
            writer.println("---------------------------------------------");
            writer.println("| No. | Category | Name | Price | Quantity |");
            writer.println("---------------------------------------------");
            
            int no = 1;
            for(ArrayList<Device> devices : devicesLists){
                for(Device device : devices){
                    writer.printf("| %2d | %-9s | %-15s | $%-6.2f | %-8d |%n", 
                                  no++, device.getCategory(), device.getName(), 
                                  device.getPrice(), device.getQuantity());
                }
            }
            
            writer.println("---------------------------------------------");
            writer.println("Summary:");
            writer.println("- Total Number of Devices: " + getTotalDeviceCount());
            writer.println("- Total Inventory Value: $" + calculateTotalValue());
            writer.println("End of Report");
            
            writer.close();
        } catch (FileNotFoundException e) {
            System.out.println("An error occurred while writing to the file.");
            e.printStackTrace();
        }
    }
    /**
    * Calculates and returns the total device number.
    * Complexity: O(n), where n is the total number of lists.
    * It potentially iterates through all the lists,
    * so making the complexity linearly dependent on the total number of lists.
    */
    private int getTotalDeviceCount() {
        int count = 0;
        for(ArrayList<Device> devices : devicesLists){
            count += devices.size();
        }
        return count;
    }



}

