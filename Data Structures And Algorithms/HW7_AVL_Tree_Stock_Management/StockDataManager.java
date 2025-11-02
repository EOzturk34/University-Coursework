import java.io.*;
import java.util.Random;
import java.util.Scanner;

/**
 * The StockDataManager class provides methods for managing stock data using an AVL tree.
 * It supports adding, removing, searching, and updating stocks, as well as generating
 * input files for batch operations and performing operations from an input file.
 */
public class StockDataManager {
    private AVLTree avlTree;

    /**
     * Constructs a new StockDataManager object and initializes the AVL tree.
     */
    public StockDataManager() {
        avlTree = new AVLTree();
    }

    /**
     * Adds a new stock to the AVL tree.
     *
     * @param symbol    The stock symbol.
     * @param price     The stock price.
     * @param volume    The stock trading volume.
     * @param marketCap The stock market capitalization.
     */
    public void addStock(String symbol, double price, long volume, long marketCap) {
        Stock stock = new Stock(symbol, price, volume, marketCap);
        avlTree.insert(stock);
    }

    /**
     * Removes a stock from the AVL tree by its symbol.
     *
     * @param symbol The symbol of the stock to remove.
     */
    public void removeStock(String symbol) {
        avlTree.delete(symbol);
    }

    /**
     * Searches for a stock in the AVL tree by its symbol.
     *
     * @param symbol The symbol of the stock to search for.
     * @return The stock if found, otherwise null.
     */
    public Stock searchStock(String symbol) {
        return avlTree.search(symbol);
    }

    /**
     * Updates a stock in the AVL tree.
     *
     * @param symbol       The current symbol of the stock to update.
     * @param newSymbol    The new symbol of the stock.
     * @param newPrice     The new price of the stock.
     * @param newVolume    The new trading volume of the stock.
     * @param newMarketCap The new market capitalization of the stock.
     */
    public void updateStock(String symbol, String newSymbol, double newPrice, long newVolume, long newMarketCap) {
        Stock stock = avlTree.search(symbol);
        if (stock != null) {
            avlTree.delete(symbol);
            avlTree.insert(new Stock(newSymbol, newPrice, newVolume, newMarketCap));
        }
    }

    /**
     * Generates an input file with random operations for testing.
     *
     * @param filename    The name of the file to generate.
     * @param nodeCount   The number of initial nodes to add.
     * @param addCount    The number of add operations.
     * @param removeCount The number of remove operations.
     * @param searchCount The number of search operations.
     * @param updateCount The number of update operations.
     */
    public void generateInputFile(String filename, int nodeCount, int addCount, int removeCount, int searchCount, int updateCount) {
        Random random = new Random();
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filename))) {
            for (int i = 0; i < nodeCount; i++) {
                String symbol = generateRandomSymbol();
                double price = random.nextDouble() * 100;
                long volume = random.nextInt(1000);
                long marketCap = random.nextInt(10000);
                writer.write(String.format("ADD %s %.2f %d %d\n", symbol, price, volume, marketCap));
            }
            for (int i = 0; i < addCount; i++) {
                String symbol = generateRandomSymbol();
                double price = random.nextDouble() * 100;
                long volume = random.nextInt(1000);
                long marketCap = random.nextInt(10000);
                writer.write(String.format("ADD %s %.2f %d %d\n", symbol, price, volume, marketCap));
            }
            for (int i = 0; i < removeCount; i++) {
                String symbol = generateRandomSymbol();
                writer.write(String.format("REMOVE %s\n", symbol));
            }
            for (int i = 0; i < searchCount; i++) {
                String symbol = generateRandomSymbol();
                writer.write(String.format("SEARCH %s\n", symbol));
            }
            for (int i = 0; i < updateCount; i++) {
                String symbol = generateRandomSymbol();
                String newSymbol = generateRandomSymbol();
                double newPrice = random.nextDouble() * 100;
                long newVolume = random.nextInt(1000);
                long newMarketCap = random.nextInt(10000);
                writer.write(String.format("UPDATE %s %s %.2f %d %d\n", symbol, newSymbol, newPrice, newVolume, newMarketCap));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Performs operations from an input file.
     *
     * @param filename The name of the input file.
     */
    public void performOperationsFromFile(String filename) {
        try (Scanner scanner = new Scanner(new File(filename))) {
            while (scanner.hasNextLine()) {
                String[] parts = scanner.nextLine().split(" ");
                String operation = parts[0];
                switch (operation) {
                    case "ADD":
                        addStock(parts[1], Double.parseDouble(parts[2]), Long.parseLong(parts[3]), Long.parseLong(parts[4]));
                        break;
                    case "REMOVE":
                        removeStock(parts[1]);
                        break;
                    case "SEARCH":
                        searchStock(parts[1]);
                        break;
                    case "UPDATE":
                        updateStock(parts[1], parts[2], Double.parseDouble(parts[3]), Long.parseLong(parts[4]), Long.parseLong(parts[5]));
                        break;
                }
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }

    /**
     * Generates a random stock symbol.
     *
     * @return A random stock symbol consisting of 3 to 5 uppercase letters.
     */
    public String generateRandomSymbol() {
        Random random = new Random();
        int length = 3 + random.nextInt(3); // 3 to 5 characters
        StringBuilder symbol = new StringBuilder(length);
        for (int i = 0; i < length; i++) {
            symbol.append((char) ('A' + random.nextInt(26)));
        }
        return symbol.toString();
    }
}
