#!/bin/bash
# simple_30_users_test.sh - Simple but effective 30 user test

SERVER_IP="127.0.0.1"
SERVER_PORT="5000"
TEST_DIR="test_30users_simple_$(date +%Y%m%d_%H%M%S)"
LOG_DIR="$TEST_DIR/logs"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# Arrays to track processes
declare -a pids

# Create test directory
mkdir -p "$LOG_DIR"

# Cleanup function
cleanup() {
    echo -e "\n${YELLOW}Cleaning up...${NC}"
    for pid in "${pids[@]}"; do
        kill $pid 2>/dev/null
    done
    sleep 1
    killall chatclient 2>/dev/null
    echo -e "${GREEN}Cleanup complete!${NC}"
}

trap cleanup EXIT

# Function to create user with predefined actions
create_user() {
    local username=$1
    local room=$2
    local actions=$3
    local log_file="$LOG_DIR/${username}.log"
    
    (
        echo "$username"
        sleep 1
        echo "/join $room"
        sleep 2
        
        # Execute additional actions if provided
        if [ ! -z "$actions" ]; then
            eval "$actions"
        fi
        
        # Keep alive for testing
        sleep 60
        echo "/exit"
    ) | ./chatclient $SERVER_IP $SERVER_PORT > "$log_file" 2>&1 &
    
    local pid=$!
    pids+=($pid)
    return $pid
}

# Print header
echo -e "${GREEN}=====================================
SIMPLE 30 USER TEST
=====================================${NC}"
echo -e "Test Directory: ${YELLOW}$TEST_DIR${NC}"
echo -e "Server: ${YELLOW}$SERVER_IP:$SERVER_PORT${NC}\n"

# SCENARIO 1: Create 30 users and distribute to rooms
echo -e "${BLUE}SCENARIO 1: Creating 30 users${NC}"
echo "================================="

# Create 5 team rooms with 5 members each
for team in {1..5}; do
    echo -e "${CYAN}Creating Team $team (5 members)...${NC}"
    
    # Team leader
    create_user "leader$team" "team$team" "sleep 3; echo '/broadcast Welcome to team $team!'"
    echo "✓ leader$team"
    sleep 0.2
    
    # Team members
    for member in {1..4}; do
        user_num=$(( (team-1)*4 + member ))
        create_user "member$user_num" "team$team" "sleep 5; echo '/broadcast Ready to work!'"
        echo "✓ member$user_num"
        sleep 0.1
    done
done

# Create 5 floating users who will switch rooms
echo -e "\n${CYAN}Creating 5 floating users...${NC}"
for i in {1..5}; do
    actions="sleep 7; echo '/join team$(( i % 5 + 1 ))'; sleep 3; echo '/broadcast Visiting from team$i!'; sleep 3; echo '/join lobby'"
    create_user "floater$i" "lobby" "$actions"
    echo "✓ floater$i"
    sleep 0.2
done

echo -e "\n${GREEN}✅ All 30 users created!${NC}"
sleep 5

# SCENARIO 2: Communication tests
echo -e "\n${BLUE}SCENARIO 2: Communication Tests${NC}"
echo "================================="

# Create some inter-team communication
echo -e "${CYAN}Testing whispers and broadcasts...${NC}"

# Leaders whisper to each other
(
    echo "leader1"
    sleep 1
    echo "/whisper leader2 Meeting at 3pm in general room"
    sleep 2
    echo "/join general"
    sleep 2
    echo "/broadcast All leaders please join general room"
    sleep 30
    echo "/exit"
) | ./chatclient $SERVER_IP $SERVER_PORT > "$LOG_DIR/leader1_comm.log" 2>&1 &
pids+=($!)

sleep 3

# SCENARIO 3: File transfers
echo -e "\n${BLUE}SCENARIO 3: File Transfer Tests${NC}"
echo "================================="

# Create test files
echo "Test content" > test_file.txt

# Multiple file transfers
echo -e "${CYAN}Initiating file transfers...${NC}"
(
    echo "filesender1"
    sleep 1
    echo "/join team1"
    sleep 1
    echo "/sendfile test_file.txt member1"
    sleep 2
    echo "/sendfile test_file.txt member2"
    sleep 2
    echo "/sendfile test_file.txt member3"
    sleep 20
    echo "/exit"
) | ./chatclient $SERVER_IP $SERVER_PORT > "$LOG_DIR/filesender1.log" 2>&1 &
pids+=($!)

echo "✓ File transfers initiated"

# Wait for activities
echo -e "\n${YELLOW}Letting users interact for 20 seconds...${NC}"
sleep 20

# SCENARIO 4: Check server capacity
echo -e "\n${BLUE}SCENARIO 4: Server Capacity Check${NC}"
echo "================================="

# Try to add one more user (31st)
echo -e "${CYAN}Attempting to add 31st user...${NC}"
(
    echo "extra_user"
    sleep 5
    echo "/exit"
) | ./chatclient $SERVER_IP $SERVER_PORT > "$LOG_DIR/extra_user.log" 2>&1 &
extra_pid=$!
pids+=($extra_pid)

sleep 3

if grep -q "Server full" "$LOG_DIR/extra_user.log" 2>/dev/null; then
    echo -e "${GREEN}✓ Server correctly rejected 31st user${NC}"
else
    echo -e "${YELLOW}⚠ 31st user might have connected (check if others disconnected)${NC}"
fi

# Statistics
echo -e "\n${BLUE}TEST STATISTICS${NC}"
echo "================================="

# Count successful connections
connected=0
for log in "$LOG_DIR"/*.log; do
    if grep -q "Welcome!" "$log" 2>/dev/null; then
        ((connected++))
    fi
done

# Count room joins
room_joins=$(grep -h "joined\|rejoined" "$LOG_DIR"/*.log 2>/dev/null | wc -l)

# Count messages
messages=$(grep -h "broadcast\|whisper" "$LOG_DIR"/*.log 2>/dev/null | wc -l)

# Count file events
files=$(grep -h "sendfile\|File" "$LOG_DIR"/*.log 2>/dev/null | wc -l)

echo -e "Connected Users: ${GREEN}$connected${NC}"
echo -e "Room Join Events: ${GREEN}$room_joins${NC}"
echo -e "Messages Sent: ${GREEN}$messages${NC}"
echo -e "File Operations: ${GREEN}$files${NC}"

# Sample logs
echo -e "\n${CYAN}Sample Activity Logs:${NC}"
echo "================================="

echo -e "\n${YELLOW}Leader1 log sample:${NC}"
grep -E "broadcast|whisper|join" "$LOG_DIR/leader1.log" 2>/dev/null | head -5

echo -e "\n${YELLOW}Member1 log sample:${NC}"
grep -E "broadcast|File|join" "$LOG_DIR/member1.log" 2>/dev/null | head -5

# Summary report
REPORT="$TEST_DIR/summary.txt"
{
    echo "30 USER TEST SUMMARY"
    echo "===================="
    echo "Date: $(date)"
    echo "Connected: $connected/30"
    echo "Room Joins: $room_joins"
    echo "Messages: $messages"
    echo "File Ops: $files"
    echo ""
    echo "Room Distribution:"
    echo "- team1-5: 5 users each"
    echo "- lobby: 5 floaters"
    echo "- general: leaders meeting"
} > "$REPORT"

echo -e "\n${GREEN}✅ Test completed!${NC}"
echo -e "Summary saved to: ${YELLOW}$REPORT${NC}"
echo -e "All logs in: ${YELLOW}$LOG_DIR/${NC}"

# Keep running briefly
echo -e "\n${YELLOW}Test will auto-cleanup in 10 seconds...${NC}"
echo "(Press Ctrl+C to cleanup immediately)"
sleep 10

# Cleanup handled by trap
