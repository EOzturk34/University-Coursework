
  /**
 * The Stock class represents a stock with its symbol, price, trading volume, and market capitalization.
 */
public class Stock {
    private String symbol;
    private double price;
    private long volume;
    private long marketCap;
    /**
     * Constructs a new Stock object with the specified symbol, price, volume, and market capitalization.
     *
     * @param symbol The stock symbol.
     * @param price The current price of the stock.
     * @param volume The current trading volume of the stock.
     * @param marketCap The market capitalization of the stock.
     */
    public Stock(String symbol, double price, long volume, long marketCap) {
        this.symbol = symbol;
        this.price = price;
        this.volume = volume;
        this.marketCap = marketCap;
    }
    /**
     * Gets the stock symbol.
     *
     * @return The stock symbol.
     */
    // Getters and Setters
    public String getSymbol() {
        return symbol;
    }
    /**
     * Sets the stock symbol.
     *
     * @param symbol The new stock symbol.
     */
    public void setSymbol(String symbol) {
        this.symbol = symbol;
    }
    /**
     * Gets the current price of the stock.
     *
     * @return The current price of the stock.
     */
    public double getPrice() {
        return price;
    }
    /**
     * Sets the current price of the stock.
     *
     * @param price The new price of the stock.
     */
    public void setPrice(double price) {
        this.price = price;
    }
    /**
     * Gets the current trading volume of the stock.
     *
     * @return The current trading volume of the stock.
     */
    public long getVolume() {
        return volume;
    }
    /**
     * Sets the current trading volume of the stock.
     *
     * @param volume The new trading volume of the stock.
     */
    public void setVolume(long volume) {
        this.volume = volume;
    }
    /**
     * Gets the market capitalization of the stock.
     *
     * @return The market capitalization of the stock.
     */
    public long getMarketCap() {
        return marketCap;
    }
    /**
     * Sets the market capitalization of the stock.
     *
     * @param marketCap The new market capitalization of the stock.
     */
    public void setMarketCap(long marketCap) {
        this.marketCap = marketCap;
    }
}
