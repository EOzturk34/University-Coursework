import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.LogarithmicAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

import javax.swing.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Random;
import java.util.Scanner;

/**
 * The PerformanceTest class measures the performance of an AVL tree implementation
 * for stock data management. It generates input files for batch operations and
 * records the time taken for add, remove, search, and update operations.
 */
public class PerformanceTest {
    private static StockDataManager manager = new StockDataManager();
    private static Random random = new Random();
    private static int operationCount = 250; // Defined operation count for quicker execution

    /**
     * The main method that initiates the performance test.
     *
     * @param args Command line arguments.
     */
    public static void main(String[] args) {
        String filename = "input.txt";
        int initialNodeCount = 1000;

        manager.generateInputFile(filename, initialNodeCount, operationCount, operationCount, operationCount, operationCount);

        int[] nodeCounts = new int[10]; // Reduced the number of node counts for quicker execution
        for (int i = 0; i < 10; i++) {
            nodeCounts[i] = (i + 1) * 5000;
        }

        XYSeries addSeries = new XYSeries("Add");
        XYSeries removeSeries = new XYSeries("Remove");
        XYSeries searchSeries = new XYSeries("Search");
        XYSeries updateSeries = new XYSeries("Update");

        for (int count : nodeCounts) {
            int addCount = count / 4;
            int removeCount = count / 4;
            int searchCount = count / 4;
            int updateCount = count / 4;
            
            manager.generateInputFile(filename, count, addCount, removeCount, searchCount, updateCount);

            long addTime = testAddPerformance(filename);
            long removeTime = testRemovePerformance(filename);
            long searchTime = testSearchPerformance(filename);
            long updateTime = testUpdatePerformance(filename);

            addSeries.add(count, addTime);
            removeSeries.add(count, removeTime);
            searchSeries.add(count, searchTime);
            updateSeries.add(count, updateTime);
        }

        XYSeriesCollection dataset = new XYSeriesCollection();
        dataset.addSeries(addSeries);
        dataset.addSeries(removeSeries);
        dataset.addSeries(searchSeries);
        dataset.addSeries(updateSeries);

        JFreeChart chart = ChartFactory.createXYLineChart(
                "AVL Tree Performance",
                "Number of Nodes",
                "Time (nanoseconds)",
                dataset,
                PlotOrientation.VERTICAL,
                true,
                true,
                false
        );

        XYPlot plot = chart.getXYPlot();
        XYLineAndShapeRenderer renderer = new XYLineAndShapeRenderer();
        plot.setRenderer(renderer);

        LogarithmicAxis yAxis = new LogarithmicAxis("Time (nanoseconds)");
        plot.setRangeAxis(yAxis);

        JFrame frame = new JFrame("Performance Test");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.add(new ChartPanel(chart));
        frame.pack();
        frame.setVisible(true);
    }

    /**
     * Tests the performance of add operations.
     *
     * @param filename The name of the file containing the operations.
     * @return The total time taken for the add operations.
     */
    private static long testAddPerformance(String filename) {
        manager = new StockDataManager();

        long totalTime = 0;
        try (Scanner scanner = new Scanner(new File(filename))) {
            while (scanner.hasNextLine()) {
                String[] parts = scanner.nextLine().split(" ");
                if (parts[0].equals("ADD")) {
                    long startTime = System.nanoTime();
                    manager.addStock(parts[1], Double.parseDouble(parts[2].replace(",", ".")), Long.parseLong(parts[3]), Long.parseLong(parts[4]));
                    long endTime = System.nanoTime();
                    totalTime += (endTime - startTime);
                }
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        return totalTime / operationCount; // Use operationCount for average
    }

    /**
     * Tests the performance of remove operations.
     *
     * @param filename The name of the file containing the operations.
     * @return The total time taken for the remove operations.
     */
    private static long testRemovePerformance(String filename) {
        manager = new StockDataManager();

        long totalTime = 0;
        try (Scanner scanner = new Scanner(new File(filename))) {
            while (scanner.hasNextLine()) {
                String[] parts = scanner.nextLine().split(" ");
                if (parts[0].equals("REMOVE")) {
                    long startTime = System.nanoTime();
                    manager.removeStock(parts[1]);
                    long endTime = System.nanoTime();
                    totalTime += (endTime - startTime);
                }
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        return totalTime / operationCount; // Use operationCount for average
    }

    /**
     * Tests the performance of search operations.
     *
     * @param filename The name of the file containing the operations.
     * @return The total time taken for the search operations.
     */
    private static long testSearchPerformance(String filename) {
        manager = new StockDataManager();

        long totalTime = 0;
        try (Scanner scanner = new Scanner(new File(filename))) {
            while (scanner.hasNextLine()) {
                String[] parts = scanner.nextLine().split(" ");
                if (parts[0].equals("SEARCH")) {
                    long startTime = System.nanoTime();
                    manager.searchStock(parts[1]);
                    long endTime = System.nanoTime();
                    totalTime += (endTime - startTime);
                }
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        return totalTime / operationCount; // Use operationCount for average
    }

    /**
     * Tests the performance of update operations.
     *
     * @param filename The name of the file containing the operations.
     * @return The total time taken for the update operations.
     */
    private static long testUpdatePerformance(String filename) {
        manager = new StockDataManager();

        long totalTime = 0;
        try (Scanner scanner = new Scanner(new File(filename))) {
            while (scanner.hasNextLine()) {
                String[] parts = scanner.nextLine().split(" ");
                if (parts[0].equals("UPDATE")) {
                    long startTime = System.nanoTime();
                    manager.updateStock(parts[1], parts[2], Double.parseDouble(parts[3].replace(",", ".")), Long.parseLong(parts[4]), Long.parseLong(parts[5]));
                    long endTime = System.nanoTime();
                    totalTime += (endTime - startTime);
                }
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        return totalTime / operationCount; // Use operationCount for average
    }
}
