import java.util.LinkedList;
import java.util.List;

/**
 * Represents a directory in the file system which can contain other directories and files.
 */
public class Directory extends FileSystemElement {
    private List<FileSystemElement> children;

    /**
     * Constructs a new Directory with the specified name and parent.
     * Initializes an empty list of children.
     * @param name the name of the directory
     * @param parent the parent file system element 
     */
    public Directory(String name, FileSystemElement parent){
        super(name, parent);
        children = new LinkedList<>();
    }

    /**
     * Adds a file system element (file or directory) to this directory.
     * @param element the file system element to add
     */
    public void addElement(FileSystemElement element) { children.add(element); }

    /**
     * Removes a file system element (file or directory) from this directory.
     * @param element the file system element to remove
     */
    public void removeElement(FileSystemElement element) { children.remove(element); }

    /**
     * Returns a list of children (files or directories) contained within this directory.
     * @return the list of children
     */
    public List<FileSystemElement> getChildren() { return children; }

    /**
     * Prints the directory and its contents recursively with indentation indicating the hierarchy.
     * @param prefix the prefix to prepend to the directory name, used for indentation
     */
    @Override
    public void print(String prefix){
        System.out.println(prefix + "Directory" + getName());
        for(FileSystemElement elem : children){
            elem.print(prefix + "  ");
        }
    }

}
