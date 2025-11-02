#!/bin/bash
# test_file_queue.sh - Test file upload queue with wait times

SERVER_IP="127.0.0.1"
SERVER_PORT="5000"
TEST_DIR="test_file_queue_$(date +%Y%m%d_%H%M%S)"
LOG_DIR="$TEST_DIR/logs"
mkdir -p "$LOG_DIR"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# Track PIDs
declare -a pids

# Cleanup
cleanup() {
    echo -e "\n${YELLOW}Cleaning up...${NC}"
    for pid in "${pids[@]}"; do
        kill $pid 2>/dev/null
    done
    rm -f test_file*.pdf
    echo -e "${GREEN}Done!${NC}"
}
trap cleanup EXIT

echo -e "${GREEN}=====================================
FILE UPLOAD QUEUE TEST
=====================================${NC}"
echo -e "Testing with: ${YELLOW}10 simultaneous file transfers${NC}"
echo -e "Queue limit: ${YELLOW}5 concurrent uploads${NC}"
echo -e "Expected: ${YELLOW}5 files process immediately, 5 wait in queue${NC}\n"

# Create test files
echo "Creating test files..."
for i in {1..10}; do
    echo "Test content for file $i" > "test_file$i.pdf"
done

# Create receiver
echo -e "${BLUE}Creating receiver...${NC}"
(
    echo "receiver"
    sleep 60  # Stay alive for full test
    echo "/exit"
) | ./chatclient $SERVER_IP $SERVER_PORT > "$LOG_DIR/receiver.log" 2>&1 &
receiver_pid=$!
pids+=($receiver_pid)

sleep 2

# Create 10 senders simultaneously
echo -e "${BLUE}Creating 10 senders simultaneously...${NC}"
start_time=$(date +%s)

for i in {1..10}; do
    (
        echo "sender$i"
        sleep 1
        echo "/sendfile test_file$i.pdf receiver"
        sleep 20  # Wait for transfer to complete
        echo "/exit"
    ) | ./chatclient $SERVER_IP $SERVER_PORT > "$LOG_DIR/sender$i.log" 2>&1 &
    
    sender_pid=$!
    pids+=($sender_pid)
    echo "✓ sender$i started"
done

echo -e "\n${CYAN}All senders created at: $(date +%H:%M:%S)${NC}"
echo -e "${YELLOW}Monitoring file transfers...${NC}\n"

# Monitor progress for 25 seconds
for second in {1..25}; do
    echo -ne "\rElapsed: ${second}s "
    
    # Check for queue messages every 5 seconds
    if [ $((second % 5)) -eq 0 ]; then
        echo ""
        
        # Count files in progress
        in_progress=$(grep -h "Upload progress" "$LOG_DIR"/sender*.log 2>/dev/null | grep "100%" | wc -l)
        echo -e "${GREEN}Files completed: $in_progress${NC}"
        
        # Check for queue position messages
        queue_msgs=$(grep -h "queue at position" "$LOG_DIR"/sender*.log 2>/dev/null | tail -5)
        if [ ! -z "$queue_msgs" ]; then
            echo -e "${YELLOW}Recent queue messages:${NC}"
            echo "$queue_msgs"
        fi
        
        echo ""
    fi
    
    sleep 1
done

echo -e "\n\n${BLUE}ANALYZING RESULTS...${NC}"
echo "================================="

# Analyze timing
echo -e "\n${CYAN}1. Queue Wait Times:${NC}"
grep -h "started upload after" /var/log/syslog 2>/dev/null | tail -10 | while read line; do
    if [[ $line =~ after\ ([0-9]+)\ seconds ]]; then
        wait_time=${BASH_REMATCH[1]}
        filename=$(echo $line | grep -oP "'\K[^']+(?=')" | head -1)
        sender=$(echo $line | grep -oP "user '\K[^']+(?=')" | tail -1)
        echo "   • $filename from $sender: waited ${wait_time}s"
    fi
done

# If no syslog, check server.log
if [ -f "server.log" ]; then
    echo -e "\n${CYAN}From server.log:${NC}"
    grep "started upload after" server.log | tail -10 | while read line; do
        if [[ $line =~ after\ ([0-9]+)\ seconds ]]; then
            wait_time=${BASH_REMATCH[1]}
            echo "   • Wait time found: ${wait_time}s"
        fi
    done
fi

# Count successful transfers
echo -e "\n${CYAN}2. Transfer Statistics:${NC}"
completed=0
queued=0
for i in {1..10}; do
    if grep -q "sent successfully" "$LOG_DIR/sender$i.log" 2>/dev/null; then
        ((completed++))
    fi
    if grep -q "position" "$LOG_DIR/sender$i.log" 2>/dev/null; then
        ((queued++))
    fi
done

echo "   • Files sent: $completed/10"
echo "   • Files queued: $queued"

# Check receiver's perspective
echo -e "\n${CYAN}3. Receiver Statistics:${NC}"
received=$(grep -c "Received" "$LOG_DIR/receiver.log" 2>/dev/null)
echo "   • Files received: $received"

# Sample sender experiences
echo -e "\n${CYAN}4. Sample Sender Experiences:${NC}"

echo -e "\n${YELLOW}Sender1 (should process immediately):${NC}"
grep -E "queue|progress|success" "$LOG_DIR/sender1.log" 2>/dev/null | head -5

echo -e "\n${YELLOW}Sender6 (should wait in queue):${NC}"
grep -E "queue|position|wait|progress" "$LOG_DIR/sender6.log" 2>/dev/null | head -5

echo -e "\n${YELLOW}Sender10 (longest wait):${NC}"
grep -E "queue|position|wait|progress" "$LOG_DIR/sender10.log" 2>/dev/null | head -5

# Calculate theoretical wait times
echo -e "\n${CYAN}5. Theoretical Wait Times:${NC}"
echo "   • Files 1-5: No wait (immediate processing)"
echo "   • File 6: ~4 seconds wait (1 file ahead)"
echo "   • File 7: ~8 seconds wait (2 files ahead)"
echo "   • File 8: ~12 seconds wait (3 files ahead)"
echo "   • File 9: ~16 seconds wait (4 files ahead)"
echo "   • File 10: ~20 seconds wait (5 files ahead)"

# Summary
end_time=$(date +%s)
total_time=$((end_time - start_time))

echo -e "\n${GREEN}TEST SUMMARY:${NC}"
echo "================================="
echo "Total test duration: ${total_time}s"
echo "Expected behavior:"
echo "  - 5 worker threads process files concurrently"
echo "  - Each file takes 4 seconds to transfer"
echo "  - Files 6-10 wait in queue based on position"
echo "  - Queue follows FIFO order"

if [ $completed -eq 10 ]; then
    echo -e "\n${GREEN}✅ TEST PASSED: All files transferred successfully!${NC}"
else
    echo -e "\n${RED}❌ TEST INCOMPLETE: Only $completed/10 files transferred${NC}"
fi

echo -e "\n${YELLOW}Check server.log for detailed queue timing information${NC}"
echo -e "Logs saved in: ${YELLOW}$LOG_DIR${NC}"

# Keep running briefly
sleep 5
