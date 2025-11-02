import java.util.Arrays;
import java.util.List;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        SocialNetwork network = new SocialNetwork();
        Scanner scanner = new Scanner(System.in);
        network.addPerson("John Doe", 25, Arrays.asList("reading", "hiking", "cooking"));
        network.addPerson("Jane Smith", 22, Arrays.asList("swimming", "cooking"));
        network.addPerson("Alice Johnson", 27, Arrays.asList("hiking", "painting"));
        network.addPerson("Bob Brown", 30, Arrays.asList("reading", "swimming"));
        network.addPerson("Emily Davis", 28, Arrays.asList("running", "swimming"));
        network.addPerson("Frank Wilson", 26, Arrays.asList("reading", "hiking"));
        
        network.addFriendship("John Doe", "Jane Smith");
        network.addFriendship("John Doe", "Alice Johnson");
        network.addFriendship("Jane Smith", "Bob Brown");
        network.addFriendship("Emily Davis", "Frank Wilson");
        System.out.println("Shortest path from John Doe to Bob Brown:");
        printPath(network.findShortestPath("John Doe", "Bob Brown"));
        
        System.out.println("\nFriend suggestions for John Doe:");
        network.suggestFriends("John Doe", 3);

        System.out.println("\nCounting clusters:");
        network.countClusters();
        
        System.err.println();
        network.removePerson("Alice Johnson");

        System.out.println("\nCounting clusters after removing Alice Johnson:");
        network.countClusters();
        while (true) {
            System.out.println("\n===== Social Network Analysis Menu =====");
            System.out.println("1. Add person");
            System.out.println("2. Remove person");
            System.out.println("3. Add friendship");
            System.out.println("4. Remove friendship");
            System.out.println("5. Find shortest path");
            System.out.println("6. Suggest friends");
            System.out.println("7. Count clusters");
            System.out.println("8. Exit");
            System.out.print("Please select an option: ");
            
            int option = scanner.nextInt();
            scanner.nextLine();  
            
            switch (option) {
                case 1:
                    System.out.print("Enter name: ");
                    String name = scanner.nextLine();
                    System.out.print("Enter age: ");
                    int age = scanner.nextInt();
                    scanner.nextLine();  
                    System.out.print("Enter hobbies (comma-separated): ");
                    String hobbiesInput = scanner.nextLine();
                    List<String> hobbies = Arrays.asList(hobbiesInput.split(","));
                    network.addPerson(name, age, hobbies);
                    break;
                case 2:
                    System.out.print("Enter name: ");
                    name = scanner.nextLine();
                    network.removePerson(name);
                    break;
                case 3:
                    System.out.print("Enter first person's name: ");
                    String name1 = scanner.nextLine();
                    System.out.print("Enter second person's name: ");
                    String name2 = scanner.nextLine();
                    network.addFriendship(name1, name2);
                    break;
                case 4:
                    System.out.print("Enter first person's name: ");
                    name1 = scanner.nextLine();
                    System.out.print("Enter second person's name: ");
                    name2 = scanner.nextLine();
                    network.removeFriendship(name1, name2);
                    break;
                case 5:
                    System.out.print("Enter start person's name: ");
                    String startName = scanner.nextLine();
                    System.out.print("Enter end person's name: ");
                    String endName = scanner.nextLine();
                    List<Person> path = network.findShortestPath(startName, endName);
                    if (!path.isEmpty()) {
                        System.out.print("Shortest path: ");
                        printPath(path);
                    } else {
                        System.out.println("No connection found between " + startName + " and " + endName);
                    }
                    break;
                case 6:
                    System.out.print("Enter person's name: ");
                    name = scanner.nextLine();
                    System.out.print("Enter number of friends to suggest: ");
                    int numSuggestions = scanner.nextInt();
                    scanner.nextLine();  
                    network.suggestFriends(name, numSuggestions);
                    break;
                case 7:
                    network.countClusters();
                    break;
                case 8:
                    System.out.println("Exiting...");
                    scanner.close();
                    return;
                default:
                    System.out.println("Invalid option. Please try again.");
                    break;
            }
        }
    }
    
    private static void printPath(List<Person> path) {
        for (int i = 0; i < path.size(); i++) {
            System.out.print(path.get(i).getName());
            if (i < path.size() - 1) {
                System.out.print(" -> ");
            }
        }
        System.out.println();
    }
}
