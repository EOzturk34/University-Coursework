import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;



/**
 * Represents a file system with directories and files.
 */
public class FileSystem {
    private Directory root;

    /**
     * Constructs a FileSystem object with an initial root directory.
     */
    public FileSystem(){
        this.root = new Directory("/",null);
    }

    /**
     * Changes the current directory to the specified path.
     * @param path the path to change to
     * @param currDir the current directory
     * @return the new current directory if path is valid, otherwise returns the original current directory
     */
    public Directory changeDirectory(String path, Directory currDir){
        path = path.substring(1);
        if(path.isEmpty()){
            System.out.println("Directory changed to: / (root)" );
            return root;
        }
        String[] parts = path.split("/");
        Directory current = root;
        Directory newCurrent = null;
        boolean flag = false;
        for(String part : parts){
            flag = false;
            for(FileSystemElement elem: current.getChildren()){
                if(elem instanceof Directory && elem.getName().equals(part)) {
                    flag = true;
                    newCurrent = (Directory) elem;
                    current = newCurrent;
                }
            }
        }

        if(flag){
            System.out.println("Directory changed to: " + path);
            return newCurrent;
        }
        System.out.println("This is inlavid path!");
        return currDir;
    }

    /**
     * Creates a new directory under the specified parent directory.
     * @param name the name of the new directory
     * @param parent the parent directory
     */
    public void createDirectory(String name, Directory parent){
        Directory newDirectory = new Directory(name, parent);
        parent.addElement(newDirectory);
        System.out.println("Directory created: " + name + '/');
    }

    /**
     * Creates a new file under the specified parent directory.
     * @param name the name of the new file
     * @param parent the parent directory
     */
    public void createFile(String name, Directory parent){
        File newFile = new File(name, parent);
        parent.addElement(newFile);
        System.out.println("File created: " + name);
    }

    /**
     * Lists all elements in a directory.
     * @param dir the directory whose contents are to be listed
     */
    public void listContents(Directory dir) {
        System.out.println("Listing contents of: " + getAbsolutePath(dir));
        for( FileSystemElement elem : dir.getChildren()){
            System.out.println((elem instanceof Directory ? "*" : "") + elem.getName() + (elem instanceof Directory ? "/" : ""));
        }
    }

    /**
     * Prints the directory tree starting from the root to current directory.
     * @param dir the directory from which to start printing
     */
    public void printDirectoryTree(Directory dir){
        LinkedList<String> list = new LinkedList<>();
        FileSystemElement current = dir;

        while (current != null) {
            list.addFirst('*' + current.getName() + (current.getName() == "/" ? "" : '/')); 
            current = current.getParent();
        }
        for(int i = 0; i < list.size(); ++i){
            for(int j = 0; j < i; ++j){
                System.out.print(" ");
            }
            System.out.print(list.get(i));
            if(i == list.size() - 1)
                System.out.print("(Current directory)" + '\n');
            else
                System.out.print('\n');
        }
        for( FileSystemElement elem : dir.getChildren()){
            for(int j = 0; j < list.size() - 1; ++j){
                System.out.print(" ");
            }
            System.out.println((elem instanceof Directory ? "*" : "") + elem.getName() + (elem instanceof Directory ? "/" : ""));
        }
    }

    /**
     * Deletes a file or directory.
     * @param name the name of the file or directory to be deleted
     * @param parent the parent directory
     */
    public void delete(String name, Directory parent) {
        FileSystemElement target = null;
        for (FileSystemElement elem : parent.getChildren()) {
            if (elem.getName().equals(name)) {
                target = elem;
                break;
            }
        }

        if (target == null) {
            System.out.println("File or directory does not exist: " + name);
            return;
        }

        if (target instanceof Directory) {
            Directory targetDir = (Directory) target;
            List<FileSystemElement> children = new ArrayList<>(targetDir.getChildren());
            for (FileSystemElement child : children) {
                delete(child.getName(), targetDir);
            }
        }

        parent.removeElement(target);
        System.out.println((target instanceof Directory ? "Directory" : "File") + " deleted: " + target.getName());
    }

    /**
     * Moves a file or directory to a new location.
     * @param name the name of the file or directory to move
     * @param path the target path where the file or directory should be moved
     * @param currentDirectory the current directory from which the element is moved
     */

    public void move(String name, String path, Directory currentDirectory) {
        Directory target = root;
        int slashCount = 0;
        String[] parts = {path.substring(1)};
        if (path.equals("/")) {
            target = root;  
        }
        for(int i = 0; i < path.length(); ++i){
            if(path.charAt(i) == '/')
                slashCount++;
        }
        if(slashCount > 1)
            parts = path.substring(1).split("/");  
        
        for (String part : parts) {
            if (part.isEmpty()) continue;  
            boolean found = false;
            for (FileSystemElement elem : target.getChildren()) {
                if (elem instanceof Directory && elem.getName().equals(part)) {
                    target = (Directory) elem;  
                    found = true;
                    break;
                }
            }
            if (!found) {
                System.out.println("Invalid target path!");
                return;
            }
        }
       
        int index = 0;
        for(FileSystemElement elem : currentDirectory.getChildren()){
            if(name.equals(elem.getName())){
                index = currentDirectory.getChildren().indexOf(elem);
                break;
            }
        }
        if(index != -1){
            FileSystemElement movedElement = currentDirectory.getChildren().remove(index);
            target.addElement(movedElement);
            System.out.println("File moved: " + name + " to " + path);
        }

    }

    

    /**
    * Sorts the contents of a directory by the date they were created.
    * The sorted list of elements is printed to the console.
    * @param dir the directory whose contents are to be sorted
    */
    public void sortContentsByDateCreated(Directory dir) {
        List<FileSystemElement> elements = dir.getChildren();

    Collections.sort(elements, new Comparator<FileSystemElement>() {
        @Override
        public int compare(FileSystemElement o1, FileSystemElement o2) {
            return o1.getDataCreated().compareTo(o2.getDataCreated());
        }
    });

    System.out.println("Contents of " + dir.getName() + " sorted by date created.");
    for (FileSystemElement elem : elements) {
        System.out.println(elem.getName() + " - Created: " + elem.getDataCreated());
    }
}

/**
 * Searches for a file or directory with the specified name within the entire file system starting from the root directory.
 * The search is recursive and prints the path of each found item.
 * @param name the name of the file or directory to find
 * @return true if the file or directory is found, false otherwise
 */
public boolean search(String name){
    return searchRecursive(name, root, "");
}

/**
 * Helper method for recursively searching a directory.
 * @param name the name of the file or directory to find
 * @param dir the current directory to search within
 * @param path the accumulated path for recursion
 * @return true if the file or directory is found in the current directory or its subdirectories
 */
private boolean searchRecursive(String name, Directory dir, String path){
    boolean found = false;

    for(FileSystemElement elem : dir.getChildren()){
        String currentPath = path.equals("/") ? path + elem.getName() : path + "/" + elem.getName();

        if(elem.getName().equals(name)){
            System.out.println("Found: " + currentPath);
            found = true;
        }
        if (elem instanceof Directory) {
            found = searchRecursive(name, (Directory) elem, currentPath) || found;
        }
    }

    return found;

}

/**
 * Returns the root directory of the file system.
 * @return the root directory
 */
    public Directory getRoot() { return root; }

    /**
    * Constructs the absolute path for a given directory.
    * This path is a concatenation of names from the root to the specified directory.
    * @param dir the directory to calculate the absolute path for
    * @return the absolute path as a StringBuilder object
    */
    public StringBuilder getAbsolutePath(Directory dir){
        StringBuilder sb = new StringBuilder();
        FileSystemElement current = dir;

        while (current != null) {
            sb.insert(0, current.getName() +(current.getName() == "/" ? "" : '/')); 
            current = current.getParent();
        }

        if(sb.length() > 1)
            sb.deleteCharAt(sb.length()-1);

        return sb;
    }
    
    
}

