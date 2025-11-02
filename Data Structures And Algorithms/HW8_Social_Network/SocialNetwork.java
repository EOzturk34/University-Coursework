import java.time.LocalDateTime;
import java.util.*;
public class SocialNetwork {
    
    Map<String, Person> people;
    Map<Person, List<Person>> graph;
    /**
     * Constructs a new SocialNetwork.
     */
    public SocialNetwork() {
        this.people = new HashMap<>();
        this.graph = new HashMap<>();
    }
    /**
     * Adds a person to the social network.
     * @param name    the name of the person
     * @param age     the age of the person
     * @param hobbies the list of hobbies of the person
     */
    public void addPerson(String name, int age, List<String> hobbies){
        privateAddPerson(name, age, hobbies);
    }

    /**
     * Private method to add a person to the social network.
     * @param name    the name of the person
     * @param age     the age of the person
     * @param hobbies the list of hobbies of the person
     */
    private void privateAddPerson(String name, int age, List<String> hobbies){
        if(people.containsKey(name)){
            System.out.println("This person already exists in the network.");
            return;
        }
        Person newPerson = new Person(name, age, hobbies, LocalDateTime.now());
        people.put(name, newPerson);
        graph.put(newPerson, new ArrayList<>());
        System.out.println("Person added: " + newPerson);
    }

    /**
     * Removes a person from the social network.
     * @param name the name of the person
     */
    public void removePerson(String name){
        privateRemovePerson(name);
    }
    /**
     * Private method to remove a person from the social network.
     * @param name the name of the person
     */
    private void privateRemovePerson(String name){
        Person removedPerson = people.remove(name);

        if(removedPerson == null){
            System.out.println("Person not found in the network");
        }

        graph.remove(removedPerson);
        for(List<Person> friends : graph.values()){
            friends.remove(removedPerson);
        }
        System.out.println("Person removed: " + removedPerson);
    }

     /**
     * Adds a friendship between two people in the social network.
     * @param name1 the name of the first person
     * @param name2 the name of the second person
     */
    public void addFriendship(String name1, String name2){
        privateAddFriendship(name1, name2);
    }

    /**
     * Private method to add a friendship between two people in the social network.
     * @param name1 the name of the first person
     * @param name2 the name of the second person
     */
    private void privateAddFriendship(String name1, String name2){
        Person person1 = people.get(name1);
        Person person2 = people.get(name2);

        if(person1 == null || person2 == null){
            System.out.println("One or both persons not found in the network.");
            return;
        }
        
        graph.get(person1).add(person2);
        graph.get(person2).add(person1);

        System.out.println("Friendship added between " + name1 + " and " + name2);
    }
   
    /**
     * Removes a friendship between two people in the social network.
     * @param name1 the name of the first person
     * @param name2 the name of the second person
     */

    public void removeFriendship(String name1, String name2){
        privateRemoveFriendship(name1, name2);
    }
     /**
     * Private method to remove a friendship between two people in the social network.
     * @param name1 the name of the first person
     * @param name2 the name of the second person
     */
    private void privateRemoveFriendship(String name1, String name2){
        Person person1 = people.get(name1);
        Person person2 = people.get(name2);

        if(person1 == null || person2 == null){
            System.out.println("One or both persons not found in the network.");
            return;
        }
        
        graph.get(person1).remove(person2);
        graph.get(person2).remove(person1);

        System.out.println("Friendship removed between " + name1 + " and " + name2);
    }

    /**
     * Finds the shortest path between two people in the social network.
     * @param startName the name of the starting person
     * @param endName the name of the ending person
     * @return the list of people in the shortest path
     */
    public List<Person> findShortestPath(String startName, String endName){
        return privateFindShortestPath(startName, endName);
    }

    /**
     * Private method to find the shortest path between two people in the social network.
     * @param startName the name of the starting person
     * @param endName the name of the ending person
     * @return the list of people in the shortest path
     */
    private List<Person> privateFindShortestPath(String startName, String endName){
        Person startPerson = people.get(startName);
        Person endPerson = people.get(endName);
        if(startPerson == null || endPerson == null){
            System.out.println("One or both persons not found in the network.");
            return Collections.emptyList();
        }

        Queue<Person> queue = new LinkedList<>();
        Map<Person, Person> predecessors = new HashMap<>();
        Set<Person> visited = new HashSet<>();

        queue.add(startPerson);
        visited.add(startPerson);

        while(!queue.isEmpty()){
            Person current = queue.poll();
            if(current.equals(endPerson)){
                break;
            }
            for(Person neighbor : graph.get(current)){
                if(!visited.contains(neighbor)){
                    visited.add(neighbor);
                    predecessors.put(neighbor, current);
                    queue.add(neighbor);
                }
            }
        }


        List<Person> path = new ArrayList<>();
        for(Person i = endPerson; i != null; i = predecessors.get(i)){
            path.add(i);
        }
        Collections.reverse(path);

        if (path.get(0).equals(startPerson)) {
            return path;
        } 
        else{
            System.out.println("No connection found between " + startName + " and " + endName);
            return Collections.emptyList();
        }
    }

    /**
     * Suggests friends for a person in the social network.
     * @param name the name of the person
     * @param countSuggestions the number of friend suggestions to make
     * @return the list of suggested friends
     */
    public List<Person> suggestFriends(String name, int countSuggestions){
        return privateSuggestFriends(name, countSuggestions);
    }
    /**
     * Private method to suggests friends for a person in the social network.
     * @param name the name of the person
     * @param countSuggestions the number of friend suggestions to make
     * @return the list of suggested friends
     */
    private List<Person> privateSuggestFriends(String name, int countSuggestions) {
        Person person = people.get(name);
        if (person == null) {
            System.out.println("Person not found in the network.");
            return Collections.emptyList();
        }
    
        Map<Person, Double> scores = new HashMap<>();
        Map<Person, Integer> mutualFriendsCount = new HashMap<>();
        Map<Person, Integer> commonHobbiesCount = new HashMap<>();
        List<Person> friends = graph.get(person);
    
        for (Person friend : friends) {
            for (Person currentPerson : graph.get(friend)) {
                if (!currentPerson.equals(person) && !friends.contains(currentPerson)) {
                    scores.put(currentPerson, scores.getOrDefault(currentPerson, 0.0) + 1);
                    mutualFriendsCount.put(currentPerson, mutualFriendsCount.getOrDefault(currentPerson, 0) + 1);
                }
            }
        }
    
        for (Person currentPerson : scores.keySet()) {
            int commonHobbies = 0;
            for (String hobby : currentPerson.getHobbies()) {
                if (person.getHobbies().contains(hobby)) {
                    scores.put(currentPerson, scores.get(currentPerson) + 0.5);
                    commonHobbies++;
                }
            }
            commonHobbiesCount.put(currentPerson, commonHobbies);
        }
    
        List<Map.Entry<Person, Double>> sortedSuggestedPersons = new ArrayList<>(scores.entrySet());
        sortedSuggestedPersons.sort((a, b) -> b.getValue().compareTo(a.getValue()));
    
        List<Person> suggPersons = new ArrayList<>();
        for (int i = 0; i < Math.min(countSuggestions, sortedSuggestedPersons.size()); ++i) {
            suggPersons.add(sortedSuggestedPersons.get(i).getKey());
        }
    
        for (Person suggested : suggPersons) {
            System.out.println("Suggested friend: " + suggested.getName() +
                    " (Score: " + scores.get(suggested) + ", " +
                    mutualFriendsCount.get(suggested) + " mutual friends, " +
                    commonHobbiesCount.get(suggested) + " common hobbies)");
        }
    
        return suggPersons;
    }

    /**
    * Counts the number of clusters (connected components) in the social network.
    * A cluster is defined as a group of people where each person is connected to at least one other person in the group.
    * @return the number of clusters in the social network
    */
    public int countClusters() {
        Set<Person> visited = new HashSet<>();
        int clusterCount = 0;

        for (Person currentPerson : people.values()) {
            if (!visited.contains(currentPerson)) {
                List<Person> cluster = privateCountClusters(currentPerson, visited);
                clusterCount++;
                System.out.println("Cluster " + clusterCount + ":");
                for (Person p : cluster) {
                    System.out.println(p.getName());
                }
                System.out.println(); 
            }
        }
    
        System.out.println("Number of clusters found: " + clusterCount);
        return clusterCount;
    }
    
    /**
    * Performs a Breadth-First Search (BFS) to find all people in the cluster starting from the given person.
    * Updates the visited set and returns the list of people in the cluster.
    * @param startPerson the person to start the BFS from
    * @param visited the set of already visited people
    * @return the list of people in the cluster
    */
    private List<Person> privateCountClusters(Person startPerson, Set<Person> visited) {
        Queue<Person> queue = new LinkedList<>();
        List<Person> cluster = new ArrayList<>();
        
        queue.add(startPerson);
        visited.add(startPerson);
        cluster.add(startPerson);
    
        while (!queue.isEmpty()) {
            Person current = queue.poll();
            for (Person neighbor : graph.get(current)) {
                if (!visited.contains(neighbor)) {
                    visited.add(neighbor);
                    queue.add(neighbor);
                    cluster.add(neighbor);
                }
            }
        }
    
        return cluster;
    }
    
    

  

}

