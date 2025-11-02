/**
 * Represents a file in the file system, which is a type of FileSystemElement.
 */
public class File extends FileSystemElement {

    /**
     * Constructs a new File with the specified name and parent.
     * @param name the name of the file
     * @param parent the parent file system element 
     */
    public File(String name, FileSystemElement parent) {
        super(name, parent);
    }

   
    @Override
    public void print(String prefix){ 
       
    }

}
