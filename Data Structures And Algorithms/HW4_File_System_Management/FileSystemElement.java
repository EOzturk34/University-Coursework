import java.sql.Timestamp;

/**
 * Abstract base class for elements in a file system, such as files and directories.
 */
public abstract class FileSystemElement {
    protected String name;
    protected Timestamp dataCreated;
    protected FileSystemElement parent;

    /**
     * Constructs a file system element with a name, parent, and sets the creation timestamp to the current time.
     * @param name the name of the file system element
     * @param parent the parent element in the file system hierarchy; this can be null if the element is the root
     */
    public FileSystemElement(String name, FileSystemElement parent){
        this.name = name;
        this.parent = parent;
        this.dataCreated = new Timestamp(System.currentTimeMillis());
    }

    /**
     * Returns the name of this file system element.
     * @return the name of the element
     */
    public String getName() { return name; }

    /**
     * Returns the creation timestamp of this file system element.
     * @return the timestamp representing the creation time
     */
    public Timestamp getDataCreated() { return dataCreated; }

    /**
     * Returns the parent of this file system element.
     * @return the parent element, or null if it is the root
     */
    public FileSystemElement getParent() {return parent; }

    /**
     * Sets the parent of this file system element.
     * @param parent the new parent element
     */
    public void setParent(FileSystemElement parent) { this.parent = parent; }

    /**
     * Abstract method to print information about the file system element with a specific prefix for formatting.
     * Implementations should define how the element should be displayed.
     * @param prefix a string prefix used for hierarchical formatting in the output
     */
    public abstract void print(String prefix);

}
