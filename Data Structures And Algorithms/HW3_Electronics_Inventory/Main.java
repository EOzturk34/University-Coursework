import java.util.Scanner;
import EmrePack.*;
// Main method's complexity cannot be directly determined as it depends 
// on user's input and the specific operations performed.
// However, for each case in the switch statement, the complexity would be as follows:
// Case 1 (Add a new device): O(1) - Adding a new device is typically 
// a constant time operation assuming no complex validations or operations are involved.
// Case 2 (Remove a device): O(n) - Removing a device requires 
// searching through the list, assuming n is the number of devices.
// Case 3 (Update device details): O(n) - Similar to removal, 
// updating details requires finding the device first.
// Case 4 (List all devices): O(n) - Listing all devices is linear with respect to the number of devices.
// Case 5 (Find the cheapest device): O(n) - Requires iterating through all devices to find the cheapest.
// Case 6 (Sort devices by price): O(n log n) - Sorting devices based on price, where n is the number of devices.
// Case 7 (Calculate total inventory value): O(n) - Summing up the price of all devices.
// Case 8 (Restock a device): O(n) - Finding a device and updating its stock involves linear search.
// Case 9 (Export inventory report): O(n) - Writing each device to a file,
// plus the time taken by calculateTotalValue which is also O(n).
// Note: The complexity of switching and repeated operations 
// is not additive across cases because each operation is independent and based on user choice.

public class Main {
    public static void main(String[] args) {
        System.out.println("Welcome to the Electronics Inventory Management System!");
        Inventory inventory = new Inventory();
        int choice;
        String category;
        String name;
        double price;
        int quantity;
        Scanner scanner = new Scanner(System.in);
        do{
            System.out.println("Please select an option:");
            System.out.print(  "1. Add a new device" + '\n'
                             + "2. Remove a device" + '\n' 
                             + "3. Update device details" + '\n'
                             + "4. List all devices" + '\n'
                             + "5. Find the cheapest device" + '\n'
                             + "6. Sort devices by price" + '\n'
                             + "7. Calculate total inventory value" + '\n'
                             + "8. Restock a device" + '\n'
                             + "9. Export inventory report" + '\n'
                             + "0. Exit" + '\n'
                             + "Please enter your choice: ");
            choice = scanner.nextInt();
            scanner.nextLine();
            switch(choice){
                case 1:
                    System.out.print("Enter category name: ");
                    category = scanner.nextLine();
                    System.out.print("Enter device name: ");
                    name = scanner.nextLine();
                    System.out.print("Enter price ($): ");
                    price = scanner.nextDouble();
                    scanner.nextLine();
                    System.out.print("Enter quantity: ");
                    quantity = scanner.nextInt();
                    scanner.nextLine();
                    inventory.addNewDevice(category, name, price, quantity);
                break;
                case 2:
                    System.out.print("Enter name of the device to remove: ");
                    name = scanner.nextLine();
                    inventory.removeDevice(name);
                break;  
                case 3:
                    System.out.print(" Enter the name of the device to update: ");
                    name = scanner.nextLine();
                    inventory.updateDeviceDetails(name);
                break;
                case 4:
                    inventory.listAllDevices();
                break;
                case 5:
                    inventory.findCheapestDevice();
                break;
                case 6:
                    inventory.sortDevices();
                break;
                case 7:
                    inventory.calculateTotalValue();
                break;
                case 8:
                    System.out.print("Enter the name of the device to restock: ");
                    name = scanner.nextLine();
                    inventory.restockDevice(name);
                case 9:
                    inventory.exportInventoryReport("report.txt");
                break;
                default:
                    break;
            }

       }while(choice != 0);
        scanner.close();
    }
}

