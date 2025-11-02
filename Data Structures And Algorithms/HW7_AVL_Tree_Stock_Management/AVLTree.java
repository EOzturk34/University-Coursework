import java.util.ArrayList;
import java.util.List;

/**
 * The AVLTree class represents a self-balancing binary search tree with
 * operations for inserting, deleting, and searching for stocks. It also
 * includes traversal methods for in-order, pre-order, and post-order traversals.
 */
public class AVLTree {
    private class Node {
        Stock stock;
        Node left, right;
        int height;

        Node(Stock stock) {
            this.stock = stock;
            height = 1;
        }
    }

    private Node root;

    /**
     * Gets the height of a node.
     *
     * @param N The node whose height is to be determined.
     * @return The height of the node, or 0 if the node is null.
     */
    private int height(Node N) {
        if (N == null)
            return 0;
        return N.height;
    }

    /**
     * Performs a right rotation on the subtree rooted at the given node.
     *
     * @param y The root of the subtree to rotate.
     * @return The new root of the rotated subtree.
     */
    private Node rightRotate(Node y) {
        Node x = y.left;
        Node T2 = x.right;

        x.right = y;
        y.left = T2;

        y.height = Math.max(height(y.left), height(y.right)) + 1;
        x.height = Math.max(height(x.left), height(x.right)) + 1;

        return x;
    }

    /**
     * Performs a left rotation on the subtree rooted at the given node.
     *
     * @param x The root of the subtree to rotate.
     * @return The new root of the rotated subtree.
     */
    private Node leftRotate(Node x) {
        Node y = x.right;
        Node T2 = y.left;

        y.left = x;
        x.right = T2;

        x.height = Math.max(height(x.left), height(x.right)) + 1;
        y.height = Math.max(height(y.left), height(y.right)) + 1;

        return y;
    }

    /**
     * Gets the balance factor of a node.
     *
     * @param N The node whose balance factor is to be determined.
     * @return The balance factor of the node, or 0 if the node is null.
     */
    private int getBalance(Node N) {
        if (N == null)
            return 0;
        return height(N.left) - height(N.right);
    }

    /**
     * Inserts a stock into the AVL tree.
     *
     * @param stock The stock to be inserted.
     */
    public void insert(Stock stock) {
        root = insertRec(root, stock);
    }

    /**
     * Recursively inserts a stock into the AVL tree.
     *
     * @param node  The root of the subtree in which to insert the stock.
     * @param stock The stock to be inserted.
     * @return The new root of the subtree.
     */
    private Node insertRec(Node node, Stock stock) {
        if (node == null)
            return new Node(stock);

        if (stock.getSymbol().compareTo(node.stock.getSymbol()) < 0)
            node.left = insertRec(node.left, stock);
        else if (stock.getSymbol().compareTo(node.stock.getSymbol()) > 0)
            node.right = insertRec(node.right, stock);
        else
            return node;

        node.height = 1 + Math.max(height(node.left), height(node.right));

        int balance = getBalance(node);

        if (balance > 1 && stock.getSymbol().compareTo(node.left.stock.getSymbol()) < 0)
            return rightRotate(node);

        if (balance < -1 && stock.getSymbol().compareTo(node.right.stock.getSymbol()) > 0)
            return leftRotate(node);

        if (balance > 1 && stock.getSymbol().compareTo(node.left.stock.getSymbol()) > 0) {
            node.left = leftRotate(node.left);
            return rightRotate(node);
        }

        if (balance < -1 && stock.getSymbol().compareTo(node.right.stock.getSymbol()) < 0) {
            node.right = rightRotate(node.right);
            return leftRotate(node);
        }

        return node;
    }

    /**
     * Searches for a stock in the AVL tree by its symbol.
     *
     * @param symbol The symbol of the stock to search for.
     * @return The stock if found, otherwise null.
     */
    public Stock search(String symbol) {
        return searchRec(root, symbol);
    }

    /**
     * Recursively searches for a stock in the AVL tree by its symbol.
     *
     * @param root   The root of the subtree in which to search.
     * @param symbol The symbol of the stock to search for.
     * @return The stock if found, otherwise null.
     */
    private Stock searchRec(Node root, String symbol) {
        if (root == null || root.stock.getSymbol().equals(symbol))
            return root != null ? root.stock : null;

        if (root.stock.getSymbol().compareTo(symbol) > 0)
            return searchRec(root.left, symbol);

        return searchRec(root.right, symbol);
    }

    /**
     * Deletes a stock from the AVL tree by its symbol.
     *
     * @param symbol The symbol of the stock to delete.
     */
    public void delete(String symbol) {
        root = deleteRec(root, symbol);
    }

    /**
     * Recursively deletes a stock from the AVL tree by its symbol.
     *
     * @param root   The root of the subtree in which to delete the stock.
     * @param symbol The symbol of the stock to delete.
     * @return The new root of the subtree.
     */
    private Node deleteRec(Node root, String symbol) {
        if (root == null)
            return root;

        if (symbol.compareTo(root.stock.getSymbol()) < 0)
            root.left = deleteRec(root.left, symbol);
        else if (symbol.compareTo(root.stock.getSymbol()) > 0)
            root.right = deleteRec(root.right, symbol);
        else {
            if ((root.left == null) || (root.right == null)) {
                Node temp = null;
                if (temp == root.left)
                    temp = root.right;
                else
                    temp = root.left;

                if (temp == null) {
                    temp = root;
                    root = null;
                } else
                    root = temp;
            } else {
                Node temp = minValueNode(root.right);
                root.stock = temp.stock;
                root.right = deleteRec(root.right, temp.stock.getSymbol());
            }
        }

        if (root == null)
            return root;

        root.height = Math.max(height(root.left), height(root.right)) + 1;

        int balance = getBalance(root);

        if (balance > 1 && getBalance(root.left) >= 0)
            return rightRotate(root);

        if (balance > 1 && getBalance(root.left) < 0) {
            root.left = leftRotate(root.left);
            return rightRotate(root);
        }

        if (balance < -1 && getBalance(root.right) <= 0)
            return leftRotate(root);

        if (balance < -1 && getBalance(root.right) > 0) {
            root.right = rightRotate(root.right);
            return leftRotate(root);
        }

        return root;
    }

    /**
     * Finds the node with the minimum value in a given subtree.
     *
     * @param node The root of the subtree.
     * @return The node with the minimum value in the subtree.
     */
    private Node minValueNode(Node node) {
        Node current = node;
        while (current.left != null)
            current = current.left;

        return current;
    }

    /**
     * Performs an in-order traversal of the AVL tree.
     *
     * @return A list of stocks in in-order traversal.
     */
    public List<Stock> inOrder() {
        List<Stock> result = new ArrayList<>();
        inOrderRec(root, result);
        return result;
    }

    /**
     * Recursively performs an in-order traversal of the AVL tree.
     *
     * @param node   The root of the subtree.
     * @param result The list to store the result of the traversal.
     */
    private void inOrderRec(Node node, List<Stock> result) {
        if (node != null) {
            inOrderRec(node.left, result);
            result.add(node.stock);
            inOrderRec(node.right, result);
        }
    }

    /**
     * Performs a pre-order traversal of the AVL tree.
     *
     * @return A list of stocks in pre-order traversal.
     */
    public List<Stock> preOrder() {
        List<Stock> result = new ArrayList<>();
        preOrderRec(root, result);
        return result;
    }

    /**
     * Recursively performs a pre-order traversal of the AVL tree.
     *
     * @param node   The root of the subtree.
     * @param result The list to store the result of the traversal.
     */
    private void preOrderRec(Node node, List<Stock> result) {
        if (node != null) {
            result.add(node.stock);
            preOrderRec(node.left, result);
            preOrderRec(node.right, result);
        }
    }

    /**
     * Performs a post-order traversal of the AVL tree.
     *
     * @return A list of stocks in post-order traversal.
     */
    public List<Stock> postOrder() {
        List<Stock> result = new ArrayList<>();
        postOrderRec(root, result);
        return result;
    }

    /**
     * Recursively performs a post-order traversal of the AVL tree.
     *
     * @param node   The root of the subtree.
     * @param result The list to store the result of the traversal.
     */
    private void postOrderRec(Node node, List<Stock> result) {
        if (node != null) {
            postOrderRec(node.left, result);
            postOrderRec(node.right, result);
            result.add(node.stock);
        }
    }
}
