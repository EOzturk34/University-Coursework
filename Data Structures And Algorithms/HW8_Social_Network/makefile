# Makefile for Social Network Java project

# Variables
JAVAC = javac
JAVA = java
JAVADOC = javadoc
SRC = .
DOC = doc
MAIN = Main

# Targets
.PHONY: all run clean docs

# Default target: compile, docs, and run
all: compile docs run

# Compile all Java files
compile:
	$(JAVAC) $(SRC)/*.java

# Run the main class
run:
	$(JAVA) $(MAIN)

# Generate Javadoc documentation
docs:
	$(JAVADOC) -d $(DOC) $(SRC)/*.java

# Clean up generated files
clean:
	rm -rf $(SRC)/*.class $(DOC)
