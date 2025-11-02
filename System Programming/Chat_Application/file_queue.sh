#!/bin/bash
# file_upload_queue_test.sh - File upload queue limit test script for chat server

SERVER_IP="127.0.0.1"
SERVER_PORT="5000"
TEST_RESULTS_DIR="test_results_file_queue"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Array to store PIDs
declare -a pids

# Create test results directory
mkdir -p "$TEST_RESULTS_DIR"

# Function to print test header
print_test_header() {
    echo -e "\n${BLUE}================================================${NC}"
    echo -e "${BLUE}TEST: $1${NC}"
    echo -e "${BLUE}================================================${NC}"
}

# Function to print result
print_result() {
    if [ "$1" == "PASS" ]; then
        echo -e "${GREEN}✅ $2${NC}"
    else
        echo -e "${RED}❌ $2${NC}"
    fi
}

# Function to create a test user that sends a file and exits
create_test_user() {
    local username=$1
    local filename=$2
    local receiver=$3
    local log_file="$TEST_RESULTS_DIR/${username}_${TIMESTAMP}.log"
    
    (
        echo "$username"
        sleep 1
        echo "/sendfile $filename $receiver"
        sleep 10  # Wait for queue processing
        echo "/exit"
    ) | ./chatclient $SERVER_IP $SERVER_PORT > "$log_file" 2>&1 &
    
    echo $!
}

# Cleanup function
cleanup() {
    echo -e "\n${YELLOW}Cleaning up...${NC}"
    for pid in ${pids[@]}; do
        kill $pid 2>/dev/null
    done
    killall chatclient 2>/dev/null
    echo "Cleanup complete!"
}

# Trap to ensure cleanup on exit
trap cleanup EXIT

# Start test
echo -e "${GREEN}Starting File Upload Queue Limit Test${NC}"
echo -e "${GREEN}Test Results Directory: $TEST_RESULTS_DIR${NC}"
echo -e "${GREEN}Timestamp: $TIMESTAMP${NC}"

# TEST: File Upload Queue Limit (10 users, 5 concurrent uploads)
print_test_header "File Upload Queue Limit (10 Users)"

# Start receiver
(
    echo "receiver"
    sleep 12
    echo "/exit"
) | ./chatclient $SERVER_IP $SERVER_PORT > "$TEST_RESULTS_DIR/receiver_${TIMESTAMP}.log" 2>&1 &
receiver_pid=$!
pids+=($receiver_pid)

sleep 1

# Start 10 senders simultaneously
for i in {1..10}; do
    pid=$(create_test_user "sender$i" "file$i.pdf" "receiver")
    pids+=($pid)
    sleep 0.1  # Small delay to simulate near-simultaneous uploads
done

# Wait for all uploads to complete
sleep 12

# Check results
pass=1
queue_notification=0
completed_uploads=0

# Check sender logs for queue notifications and completion
for i in {1..10}; do
    log_file="$TEST_RESULTS_DIR/sender${i}_${TIMESTAMP}.log"
    
    # Check if file was added to queue
    if grep -q "added to upload queue" "$log_file" 2>/dev/null; then
        queue_notification=1
    fi
    
    # Check if upload completed
    if grep -q "sent successfully" "$log_file" 2>/dev/null; then
        ((completed_uploads++))
    fi
done

# Check receiver log for received files
for i in {1..10}; do
    if grep -q "file$i.pdf" "$TEST_RESULTS_DIR/receiver_${TIMESTAMP}.log" 2>/dev/null; then
        continue
    else
        pass=0
        break
    fi
done

# Check server log for queue size reaching 5
server_log="server.log"
if grep -q "Queue size: 5" "$server_log" 2>/dev/null; then
    queue_notification=1
else
    pass=0
fi

# Verify results
if [ $queue_notification -eq 1 ] && [ $completed_uploads -eq 10 ] && [ $pass -eq 1 ]; then
    print_result "PASS" "File upload queue handled 10 uploads correctly (5 concurrent)"
else
    print_result "FAIL" "File upload queue test failed (Queue notifications: $queue_notification, Completed: $completed_uploads)"
fi

# Print summary
echo -e "\n${BLUE}================================================${NC}"
echo -e "${BLUE}TEST SUMMARY${NC}"
echo -e "${BLUE}================================================${NC}"
echo -e "Queue notifications observed: $queue_notification"
echo -e "Completed uploads: $completed_uploads"
echo -e "All files received by receiver: $pass"
echo -e "\nTest logs saved in: ${YELLOW}$TEST_RESULTS_DIR${NC}"
echo -e "Check server.log for queue size entries."

# Show sample server log
echo -e "\n${YELLOW}Sample server log output:${NC}"
echo "---"
tail -n 5 "$server_log" 2>/dev/null
echo "---"

echo -e "\n${YELLOW}Test completed!${NC}"
