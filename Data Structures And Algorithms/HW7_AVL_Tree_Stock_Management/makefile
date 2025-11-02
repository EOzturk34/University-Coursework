# Project directories
SRC_DIR := .
LIB_DIR := /Users/emreozturk/Desktop/jfreechart-1.0.19
BIN_DIR := bin
DOC_DIR := doc

# Classpath for the libraries
CLASSPATH := $(LIB_DIR)/jfreechart-1.0.19-demo.jar

# Java compiler and options
JAVAC := javac
JAVA := java
JFLAGS := -cp $(CLASSPATH):$(SRC_DIR)
JVMFLAGS := -Xint -cp $(BIN_DIR):$(CLASSPATH)
JAVADOC := javadoc
JAVADOC_FLAGS := -d $(DOC_DIR) -sourcepath $(SRC_DIR) -classpath $(CLASSPATH)

# Source files
SOURCES := $(SRC_DIR)/AVLTree.java $(SRC_DIR)/Stock.java $(SRC_DIR)/StockDataManager.java $(SRC_DIR)/PerformanceTest.java
CLASSES := $(BIN_DIR)/AVLTree.class $(BIN_DIR)/Stock.class $(BIN_DIR)/StockDataManager.class $(BIN_DIR)/PerformanceTest.class

# Default target
all: $(CLASSES) run javadoc

# Rule to compile .java files to .class files
$(BIN_DIR)/AVLTree.class: $(SRC_DIR)/AVLTree.java $(BIN_DIR)/Stock.class
	@mkdir -p $(BIN_DIR)
	$(JAVAC) $(JFLAGS) -d $(BIN_DIR) $(SRC_DIR)/AVLTree.java

$(BIN_DIR)/Stock.class: $(SRC_DIR)/Stock.java
	@mkdir -p $(BIN_DIR)
	$(JAVAC) $(JFLAGS) -d $(BIN_DIR) $(SRC_DIR)/Stock.java

$(BIN_DIR)/StockDataManager.class: $(SRC_DIR)/StockDataManager.java $(BIN_DIR)/AVLTree.class $(BIN_DIR)/Stock.class
	@mkdir -p $(BIN_DIR)
	$(JAVAC) $(JFLAGS) -d $(BIN_DIR) $(SRC_DIR)/StockDataManager.java

$(BIN_DIR)/PerformanceTest.class: $(SRC_DIR)/PerformanceTest.java $(BIN_DIR)/StockDataManager.class $(BIN_DIR)/AVLTree.class $(BIN_DIR)/Stock.class
	@mkdir -p $(BIN_DIR)
	$(JAVAC) $(JFLAGS) -d $(BIN_DIR) $(SRC_DIR)/PerformanceTest.java

# Clean up build artifacts
clean:
	rm -rf $(BIN_DIR) $(DOC_DIR)

# Run the program
run:
	$(JAVA) $(JVMFLAGS) PerformanceTest

# Generate Javadoc
javadoc:
	@mkdir -p $(DOC_DIR)
	$(JAVADOC) $(JAVADOC_FLAGS) $(SRC_DIR)/*.java

.PHONY: all clean run javadoc
