import java.time.LocalDateTime;
import java.util.List;

public class Person{

    private String name;
    private int age;
    private List<String> hobbies;
    private LocalDateTime timestamp;
    /**
     * Constructs a new Person with the given name, age, hobbies, and timestamp.
     * @param name      the name of the person
     * @param age       the age of the person
     * @param hobbies   the list of hobbies of the person
     * @param timestamp the timestamp of when the person was added to the network
     */
    Person(String name, int age, List<String> hobbies, LocalDateTime timestamp){
        this.name = name;
        this.age = age;
        this.hobbies = hobbies;
        this.timestamp = timestamp;
    }

    /**
     * Sets the name of the person.
     * @param name the new name of the person
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Sets the age of the person.
     * @param age the new age of the person
     */
    public void setAge(int age) {
        this.age = age;
    }

    /**
     * Sets the hobbies of the person.
     * @param hobbies the new list of hobbies of the person
     */
    public void setHobbies(List<String> hobbies) {
        this.hobbies = hobbies;
    }

    /**
     * Sets the timestamp of when the person was added to the network.
     * @param timestamp the new timestamp
     */
    public void setTimestamp(LocalDateTime timestamp) {
        this.timestamp = timestamp;
    }

    /**
     * Returns the name of the person.
     * @return the name of the person
     */
    public String getName() {
        return name;
    }

    /**
     * Returns the age of the person.
     *@return the age of the person
     */
    public int getAge() {
        return age;
    }

    /**
     * Returns the list of hobbies of the person.
     *@return the list of hobbies of the person
     */
    public List<String> getHobbies() {
        return hobbies;
    }

    /**
     * Returns the timestamp of when the person was added to the network.
     * @return the timestamp of when the person was added to the network
     */
    public LocalDateTime getTimestamp() {
        return timestamp;
    }

    /**
     * Returns a string representation of the person.
     * @return a string representation of the person
     */
    @Override
    public String toString() {
        return "Person = " +
                "Name: '" + name +
                "', Age: " + age +
                ", Hobbies: " + hobbies +
                ", Timestamp: " + timestamp;
    }



}