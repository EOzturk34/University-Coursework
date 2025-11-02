import java.util.Scanner;

public class Main{
    private static FileSystem fs = new FileSystem();

    private static Scanner scanner = new Scanner(System.in);

    private static Directory currentDirectory = fs.getRoot();
    public static void main(String args[]){
        int choice;
        String fileOrDirectory;
        String name;
        String path;
        do{
            System.out.println("===== File System Management Menu =====" + '\n' +
            "1. Change directory" + '\n' +
            "2. List directory contents" + '\n' +
            "3. Create file/directory" + '\n' +
            "4. Delete file/directory" + '\n' +
            "5. Move file/directory" + '\n' +
            "6. Search file/directory" + '\n' +
            "7. Print directory tree" + '\n' +
            "8. Sort contents by date created" + '\n' +
            "9. Exit");
            System.out.print("Please select an option: ");
            choice = scanner.nextInt();
            switch(choice){
                case 1 :
                    System.out.print("Current Directory: " + fs.getAbsolutePath(currentDirectory) + '\n'
                    + "Enter new directory path: ");
                    scanner.nextLine();
                    path = scanner.nextLine();
                    currentDirectory = fs.changeDirectory(path, currentDirectory);
                break;
                case 2 : 
                    fs.listContents(currentDirectory);
                break;
                case 3 :
                    System.out.print("Current directory: " + fs.getAbsolutePath(currentDirectory)
                    + '\n' + "Create file or directory (f/d): ");
                    scanner.nextLine();
                    fileOrDirectory = scanner.nextLine();
                    if("d".equals(fileOrDirectory)){
                        System.out.print("Enter name for new directory: ");
                        name = scanner.nextLine();
                        fs.createDirectory(name, currentDirectory);
                    }
                    else if("f".equals(fileOrDirectory)){
                        System.out.print("Enter name for new file: ");
                        name = scanner.nextLine();
                        fs.createFile(name, currentDirectory);
                    }

                break;
                case 4 : 
                    System.out.println("Current directory: " + fs.getAbsolutePath(currentDirectory)
                    + '\n' + "Delete file or directory (f/d): ");
                    System.out.println("Enter name of directory/file to delete: ");
                    scanner.nextLine();
                    name = scanner.nextLine();
                    fs.delete(name, currentDirectory);
                break;
                case 5 :
                    System.out.println("Current directory: " + fs.getAbsolutePath(currentDirectory) + '\n'
                    + "Enter the name of file/directory to move: ");
                    scanner.nextLine();
                    name = scanner.nextLine();
                    System.out.println("Enter new directory path: ");
                    //scanner.nextLine();
                    path = scanner.nextLine();
                    fs.move(name, path, currentDirectory);
                break;
                case 6 :
                    System.out.println("Search query: ");
                    scanner.nextLine();
                    name = scanner.nextLine();
                    System.out.println("Searching from root..");
                    if(!fs.search(name)){
                        System.out.println("There is no directory or file named " + name);
                    }
                break;
                case 7 :
                    fs.printDirectoryTree(currentDirectory);
                break;
                case 8 : 
                    fs.sortContentsByDateCreated(currentDirectory);
                break;
            }


        }while(choice != 9);
    }

    
}

