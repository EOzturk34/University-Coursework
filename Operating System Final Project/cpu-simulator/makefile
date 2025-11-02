# GTU-C312 CPU Simulator Makefile

# Python interpreter
PYTHON = python3

# Program files
CPU_SIM = cpu.py
TEST_PROG = test.txt
OS_PROG = os.txt
OS_ADV = os_advanced.txt

# Default target - run simple test
all: test

# Run simple test program
test:
	@echo "=== Running Simple Test Program ==="
	$(PYTHON) $(CPU_SIM) $(TEST_PROG) -D 0

# Run operating system with threads
os:
	@echo "=== Running Operating System with Threads ==="
	$(PYTHON) $(CPU_SIM) $(OS_PROG) -D 0

# Run advanced OS
os-adv:
	@echo "=== Running Advanced Operating System ==="
	$(PYTHON) $(CPU_SIM) $(OS_ADV) -D 0

# Run with debug mode 1 (memory dump after each instruction)
debug:
	@echo "=== Running Test Program in Debug Mode 1 ==="
	$(PYTHON) $(CPU_SIM) $(TEST_PROG) -D 1

# Run with debug mode 2 (step by step)
step:
	@echo "=== Running Test Program in Step Mode ==="
	$(PYTHON) $(CPU_SIM) $(TEST_PROG) -D 2

# Run OS with debug mode 1
os-debug:
	@echo "=== Running OS in Debug Mode 1 ==="
	$(PYTHON) $(CPU_SIM) $(OS_PROG) -D 1

# Run OS with thread table debug (mode 3)
os-thread:
	@echo "=== Running OS with Thread Table Debug ==="
	$(PYTHON) $(CPU_SIM) $(OS_PROG) -D 3

# Run advanced OS with thread debug
os-adv-thread:
	@echo "=== Running Advanced OS with Thread Debug ==="
	$(PYTHON) $(CPU_SIM) $(OS_ADV) -D 3

# Create all test files
create-tests:
	@echo "Creating test files..."
	@echo "Run 'make test' for simple test"
	@echo "Run 'make os' for basic OS"
	@echo "Run 'make os-adv' for advanced OS"

# Clean up any generated files
clean:
	@echo "=== Cleaning up ==="
	find . -name "*.pyc" -delete
	find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
	find . -name "*.log" -delete
	find . -name "*.tmp" -delete
	@echo "Clean completed."

# Check if Python is available
check:
	@echo "=== Checking Python installation ==="
	@$(PYTHON) --version
	@echo "Python is available."

# Help target
help:
	@echo "GTU-C312 CPU Simulator - Available targets:"
	@echo ""
	@echo "  make test         - Run simple test program (default)"
	@echo "  make os           - Run operating system with threads"
	@echo "  make os-adv       - Run advanced OS with better scheduling"
	@echo "  make debug        - Run test with memory debug (mode 1)"
	@echo "  make step         - Run test step by step (mode 2)"
	@echo "  make os-debug     - Run OS with memory debug"
	@echo "  make os-thread    - Run OS with thread table debug"
	@echo "  make os-adv-thread - Run advanced OS with thread debug"
	@echo "  make clean        - Remove temporary files"
	@echo "  make check        - Check Python installation"
	@echo "  make help         - Show this help"
	@echo ""
	@echo "Manual usage:"
	@echo "  $(PYTHON) $(CPU_SIM) <program.txt> -D <0|1|2|3>"
	@echo ""
	@echo "Debug modes:"
	@echo "  0 - Show memory dump after program completes"
	@echo "  1 - Show memory dump after each instruction"
	@echo "  2 - Step-by-step execution (press Enter)"
	@echo "  3 - Show thread table on context switches"

# Run all tests
test-all: test os os-adv
	@echo "=== All tests completed ==="

# Prevent make from treating these as file targets
.PHONY: all test os os-adv debug step os-debug os-thread os-adv-thread clean check help create-tests test-all