#!/usr/bin/env python3

import socket
import threading
import time
import signal
import sys
import os
import queue
import random
from datetime import datetime
from collections import defaultdict

# Constants
MAX_CLIENTS = 30
MAX_ROOMS = 10
MAX_ROOM_CAPACITY = 15
MAX_USERNAME_LEN = 16
MAX_ROOM_NAME_LEN = 32
MAX_MESSAGE_LEN = 1024
MAX_FILENAME_LEN = 256
MAX_FILE_SIZE = 3 * 1024 * 1024  # 3MB
MAX_UPLOAD_QUEUE = 5
LOG_FILE = "server.log"

# ANSI color codes
COLOR_RED = "\033[31m"
COLOR_GREEN = "\033[32m"
COLOR_YELLOW = "\033[33m"
COLOR_BLUE = "\033[34m"
COLOR_CYAN = "\033[36m"
COLOR_RESET = "\033[0m"


class Client:
    def __init__(self, socket, address):
        self.socket = socket
        self.address = address
        self.username = ""
        self.current_room = ""
        self.previous_rooms = []
        self.active = True
        self.thread = None


class Room:
    def __init__(self, name):
        self.name = name
        self.clients = []
        self.lock = threading.Lock()


class FileUpload:
    def __init__(self, filename, sender, receiver, size):
        self.filename = filename
        self.sender = sender
        self.receiver = receiver
        self.size = size
        self.enqueue_time = time.time()


class ChatServer:
    def __init__(self, port):
        self.port = port
        self.server_socket = None
        self.server_running = True
        self.clients = {}
        self.rooms = {}
        self.upload_queue = queue.Queue()
        self.received_files = defaultdict(list)  # Track received files per user
        
        # Locks
        self.clients_lock = threading.Lock()
        self.rooms_lock = threading.Lock()
        self.log_lock = threading.Lock()
        
        # Set up signal handler
        signal.signal(signal.SIGINT, self.signal_handler)
        
    def log_activity(self, message):
        with self.log_lock:
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            log_entry = f"{timestamp} - {message}\n"
            with open(LOG_FILE, "a") as f:
                f.write(log_entry)
    
    def format_file_size(self, bytes_size):
        if bytes_size < 1024:
            return f"{bytes_size} bytes"
        elif bytes_size < 1024 * 1024:
            return f"{bytes_size / 1024:.2f} KB"
        else:
            return f"{bytes_size / (1024 * 1024):.2f} MB"
    
    def send_message(self, client_socket, message):
        try:
            client_socket.send(message.encode('utf-8'))
        except:
            pass
    
    def validate_username(self, username):
        if len(username) == 0 or len(username) > MAX_USERNAME_LEN:
            return False
        return username.isalnum()
    
    def validate_room_name(self, room_name):
        if len(room_name) == 0 or len(room_name) > MAX_ROOM_NAME_LEN:
            return False
        return room_name.isalnum()
    
    def find_client_by_username(self, username):
        with self.clients_lock:
            for client in self.clients.values():
                if client.username == username:
                    return client
        return None
    
    def start(self):
        # Initialize server
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
        try:
            self.server_socket.bind(('', self.port))
            self.server_socket.listen(MAX_CLIENTS)
            print(f"[INFO] Server listening on port {self.port}...")
            self.log_activity(f"[INFO] Server listening on port {self.port}...")
            
            # Start file upload worker threads
            for i in range(MAX_UPLOAD_QUEUE):
                worker = threading.Thread(target=self.file_upload_worker)
                worker.daemon = True
                worker.start()
                print(f"[INFO] File upload worker thread {i + 1} started")
                self.log_activity(f"[INFO] File upload worker thread {i + 1} started")
            
            # Accept connections
            while self.server_running:
                try:
                    client_socket, client_address = self.server_socket.accept()
                    
                    # Check if server is full
                    with self.clients_lock:
                        if len(self.clients) >= MAX_CLIENTS:
                            self.send_message(client_socket, "[ERROR] Server full. Try again later.\n")
                            print(f"[REJECTED] Connection rejected from {client_address[0]} - Server full (max {MAX_CLIENTS} clients)")
                            self.log_activity(f"[REJECTED] Connection rejected from {client_address[0]} - Server full")
                            client_socket.close()
                            continue
                    
                    # Create client object
                    client = Client(client_socket, client_address)
                    
                    # Start client handler thread
                    client_thread = threading.Thread(target=self.handle_client, args=(client,))
                    client_thread.daemon = True
                    client.thread = client_thread
                    client_thread.start()
                    
                except socket.error:
                    if self.server_running:
                        continue
                    break
                    
        except Exception as e:
            print(f"Server error: {e}")
        finally:
            self.server_socket.close()
    
    def handle_client(self, client):
        # Get username
        username_accepted = False
        max_attempts = 3
        
        while not username_accepted and max_attempts > 0:
            self.send_message(client.socket, "Enter your username: ")
            
            try:
                data = client.socket.recv(MAX_USERNAME_LEN + 20).decode('utf-8').strip()
                if not data:
                    break
                
                # Check length first
                if len(data) > MAX_USERNAME_LEN:
                    self.send_message(client.socket, COLOR_RED + "[ERROR] Username too long. Maximum 16 characters allowed.\n" + COLOR_RESET)
                    self.log_activity(f"[REJECTED] Username too long attempted: {data} (length: {len(data)})")
                    max_attempts -= 1
                    continue
                
                # Validate username
                if not self.validate_username(data):
                    self.send_message(client.socket, COLOR_RED + "[ERROR] Invalid username. Use alphanumeric characters only (max 16). Try again.\n" + COLOR_RESET)
                    max_attempts -= 1
                    continue
                
                # Check duplicate
                if self.find_client_by_username(data):
                    self.send_message(client.socket, COLOR_RED + "[ERROR] Username already taken. Please choose another.\n" + COLOR_RESET)
                    self.log_activity(f"[REJECTED] Duplicate username attempted: {data}")
                    max_attempts -= 1
                    continue
                
                # Username accepted
                client.username = data
                username_accepted = True
                
                with self.clients_lock:
                    self.clients[client.socket] = client
                
            except:
                break
        
        if not username_accepted:
            self.send_message(client.socket, COLOR_RED + "[ERROR] Maximum username attempts exceeded. Disconnecting.\n" + COLOR_RESET)
            self.cleanup_client(client)
            return
        
        self.send_message(client.socket, COLOR_GREEN + "[Server]: Welcome! Type /help for commands.\n" + COLOR_RESET)
        
        # Log connection
        print(f"[CONNECT] Client connected: user={client.username} from {client.address[0]}")
        self.log_activity(f"[LOGIN] user '{client.username}' connected from {client.address[0]}")
        
        # Main client loop
        while self.server_running and client.active:
            try:
                data = client.socket.recv(MAX_MESSAGE_LEN).decode('utf-8').strip()
                if not data:
                    break
                
                # Parse command
                if data.startswith('/'):
                    parts = data.split(' ', 2)
                    command = parts[0]
                    
                    if command == '/join' and len(parts) > 1:
                        room_name = parts[1]
                        print(f"[COMMAND] {client.username} joined room '{room_name}'")
                        self.handle_join(client, room_name)
                    
                    elif command == '/leave':
                        self.handle_leave(client)
                    
                    elif command == '/broadcast' and len(parts) > 1:
                        message = data[len('/broadcast '):]
                        print(f"[COMMAND] {client.username} broadcasted to '{client.current_room}'")
                        self.handle_broadcast(client, message)
                    
                    elif command == '/whisper' and len(parts) > 2:
                        target_parts = parts[1:3] if len(parts) == 3 else parts[1].split(' ', 1) + [parts[2]]
                        target_user = target_parts[0]
                        message = ' '.join(target_parts[1:]) if len(target_parts) > 1 else parts[2]
                        print(f"[COMMAND] {client.username} sent whisper to {target_user}")
                        self.handle_whisper(client, target_user, message)
                    
                    elif command == '/sendfile' and len(parts) > 2:
                        filename = parts[1]
                        target_user = parts[2]
                        print(f"[COMMAND] {client.username} initiated file transfer to {target_user}")
                        self.handle_sendfile(client, filename, target_user)
                    
                    elif command == '/exit':
                        self.handle_exit(client)
                        break
                    
                    elif command == '/help':
                        help_text = ("Commands:\n"
                                   "/join <room_name> - Join or create a room\n"
                                   "/leave - Leave current room\n"
                                   "/broadcast <message> - Send message to room\n"
                                   "/whisper <username> <message> - Send private message\n"
                                   "/sendfile <filename> <username> - Send file to user\n"
                                   "/exit - Disconnect from server\n")
                        self.send_message(client.socket, help_text)
                    
                    else:
                        self.send_message(client.socket, COLOR_RED + "[Error]: Unknown command\n" + COLOR_RESET)
                
                else:
                    self.send_message(client.socket, COLOR_YELLOW + "[Info]: Commands start with '/'. Type /help for help.\n" + COLOR_RESET)
                    
            except:
                break
        
        self.cleanup_client(client)
    
    def handle_join(self, client, room_name):
        if not self.validate_room_name(room_name):
            self.send_message(client.socket, COLOR_RED + "[Error]: Invalid room name. Use alphanumeric characters only (max 32).\n" + COLOR_RESET)
            return
        
        with self.rooms_lock:
            # Create room if it doesn't exist
            if room_name not in self.rooms:
                self.rooms[room_name] = Room(room_name)
            
            room = self.rooms[room_name]
        
        with room.lock:
            # Check room capacity
            if len(room.clients) >= MAX_ROOM_CAPACITY:
                self.send_message(client.socket, COLOR_RED + "[Error]: Room is full (max 15 users per room).\n" + COLOR_RESET)
                self.log_activity(f"[ROOM-FULL] user '{client.username}' tried to join full room '{room_name}'")
                return
            
            # Leave current room if in one
            previous_room = ""
            switching_rooms = False
            if client.current_room:
                previous_room = client.current_room
                switching_rooms = True
                self.remove_client_from_room(client)
                
                # Notify others in old room
                self.broadcast_room(previous_room, f"[Server]: {client.username} left the room", None)
            
            # Check if rejoining
            is_rejoin = room_name in client.previous_rooms
            
            # Add client to room
            room.clients.append(client)
            client.current_room = room_name
            
            # Add to visited rooms
            if room_name not in client.previous_rooms:
                client.previous_rooms.append(room_name)
        
        # Send confirmation
        action = "rejoined" if is_rejoin else "joined"
        self.send_message(client.socket, COLOR_GREEN + f"[Server]: You {action} the room '{room_name}'\n" + COLOR_RESET)
        
        # Notify others
        self.broadcast_room(room_name, f"[Server]: {client.username} {action} the room", None)
        
        # Log appropriately
        if switching_rooms:
            self.log_activity(f"[ROOM] user '{client.username}' left room '{previous_room}', joined '{room_name}'")
        elif is_rejoin:
            self.log_activity(f"[ROOM] user '{client.username}' rejoined '{room_name}'")
        else:
            self.log_activity(f"[JOIN] user '{client.username}' joined room '{room_name}'")
    
    def handle_leave(self, client):
        if not client.current_room:
            self.send_message(client.socket, COLOR_YELLOW + "[Info]: You are not in any room.\n" + COLOR_RESET)
            return
        
        room_name = client.current_room
        self.remove_client_from_room(client)
        
        self.send_message(client.socket, COLOR_GREEN + f"[Server]: You left the room '{room_name}'\n" + COLOR_RESET)
        
        # Notify others
        self.broadcast_room(room_name, f"[Server]: {client.username} left the room", None)
        
        self.log_activity(f"[LEAVE] user '{client.username}' left room '{room_name}'")
    
    def remove_client_from_room(self, client):
        if not client.current_room:
            return
        
        with self.rooms_lock:
            if client.current_room in self.rooms:
                room = self.rooms[client.current_room]
                with room.lock:
                    if client in room.clients:
                        room.clients.remove(client)
                    
                    # Delete room if empty
                    if len(room.clients) == 0:
                        del self.rooms[client.current_room]
        
        client.current_room = ""
    
    def broadcast_room(self, room_name, message, sender):
        with self.rooms_lock:
            if room_name not in self.rooms:
                return
            
            room = self.rooms[room_name]
            
        with room.lock:
            for client in room.clients:
                if client.active and (not sender or client.username != sender):
                    if sender:
                        formatted_msg = f"[{sender}]: {message}\n"
                    else:
                        formatted_msg = f"{message}\n"
                    self.send_message(client.socket, formatted_msg)
    
    def handle_broadcast(self, client, message):
        if not client.current_room:
            self.send_message(client.socket, COLOR_RED + "[Error]: You must join a room first.\n" + COLOR_RESET)
            return
        
        if not message:
            self.send_message(client.socket, COLOR_RED + "[Error]: Message cannot be empty.\n" + COLOR_RESET)
            return
        
        self.send_message(client.socket, COLOR_GREEN + "[Server]: Message sent to room\n" + COLOR_RESET)
        self.broadcast_room(client.current_room, message, client.username)
        
        self.log_activity(f"[BROADCAST] user '{client.username}': {message}")
    
    def handle_whisper(self, client, target_user, message):
        if not target_user or not message:
            self.send_message(client.socket, COLOR_RED + "[Error]: Usage: /whisper <username> <message>\n" + COLOR_RESET)
            return
        
        if client.username == target_user:
            self.send_message(client.socket, COLOR_RED + "[Error]: You cannot whisper to yourself.\n" + COLOR_RESET)
            return
        
        target = self.find_client_by_username(target_user)
        if not target:
            self.send_message(client.socket, COLOR_RED + "[Error]: User not found.\n" + COLOR_RESET)
            return
        
        formatted_msg = COLOR_BLUE + f"[Whisper from {client.username}]: {message}\n" + COLOR_RESET
        self.send_message(target.socket, formatted_msg)
        
        self.send_message(client.socket, COLOR_GREEN + "[Server]: Whisper sent\n" + COLOR_RESET)
        self.log_activity(f"[WHISPER] from '{client.username}' to '{target_user}': {message}")
    
    def check_duplicate_file(self, receiver_username, filename, sender):
        files = self.received_files.get(receiver_username, [])
        for f in files:
            if f['filename'] == filename and f['sender'] != sender:
                return True
        return False
    
    def add_received_file(self, receiver_username, filename, sender):
        self.received_files[receiver_username].append({
            'filename': filename,
            'sender': sender,
            'timestamp': time.time()
        })
    
    def handle_sendfile(self, client, filename, target_user):
        if not filename or not target_user:
            self.send_message(client.socket, COLOR_RED + "[Error]: Usage: /sendfile <filename> <username>\n" + COLOR_RESET)
            return
        
        # Check file extension
        ext = os.path.splitext(filename)[1].lower()
        if ext not in ['.txt', '.pdf', '.jpg', '.png']:
            self.send_message(client.socket, COLOR_RED + "[Error]: Invalid file type. Allowed: .txt, .pdf, .jpg, .png\n" + COLOR_RESET)
            return
        
        # Check if trying to send to self
        if client.username == target_user:
            self.send_message(client.socket, COLOR_RED + "[Error]: You cannot send files to yourself.\n" + COLOR_RESET)
            return
        
        target = self.find_client_by_username(target_user)
        if not target:
            self.send_message(client.socket, COLOR_RED + "[Error]: User not found.\n" + COLOR_RESET)
            return
        
        # Check for duplicate file
        if self.check_duplicate_file(target_user, filename, client.username):
            msg = COLOR_RED + f"[Error]: {target_user} already received a file named '{filename}' from another user. Please rename your file.\n" + COLOR_RESET
            self.send_message(client.socket, msg)
            self.log_activity(f"[FILE] Conflict: '{filename}' already sent to {target_user} by another user")
            return
        
        # Get file info (simulate if file doesn't exist)
        try:
            file_stat = os.stat(filename)
            file_size = file_stat.st_size
            mod_time = datetime.fromtimestamp(file_stat.st_mtime).strftime("%Y-%m-%d %H:%M:%S")
            
            # Get file type description
            file_types = {'.txt': 'Text Document', '.pdf': 'PDF Document', 
                         '.jpg': 'JPEG Image', '.png': 'PNG Image'}
            file_type = file_types.get(ext, 'Unknown')
            
            # Send file info to sender
            file_info = (COLOR_CYAN + "[File Info]\n"
                        f"  Name: {filename}\n"
                        f"  Type: {file_type}\n"
                        f"  Size: {self.format_file_size(file_size)}\n"
                        f"  Modified: {mod_time}\n"
                        f"  Permissions: {oct(file_stat.st_mode)[-3:]}\n" + COLOR_RESET)
            
            self.send_message(client.socket, file_info)
            
            # Notify receiver
            file_info = (COLOR_BLUE + "[Incoming File Alert]\n"
                        f"  From: {client.username}\n"
                        f"  File: {filename}\n"
                        f"  Type: {file_type}\n"
                        f"  Size: {self.format_file_size(file_size)}\n"
                        f"  Modified: {mod_time}\n"
                        "  Status: Waiting in queue...\n" + COLOR_RESET)
            
            self.send_message(target.socket, file_info)
            
        except OSError:
            # File doesn't exist - simulate
            file_size = 100 * 1024 + random.randint(0, MAX_FILE_SIZE - 100 * 1024)
            msg = COLOR_YELLOW + f"[Warning]: File not found locally. Using simulated size: {self.format_file_size(file_size)}\n" + COLOR_RESET
            self.send_message(client.socket, msg)
        
        if file_size > MAX_FILE_SIZE:
            self.send_message(client.socket, COLOR_RED + "[Error]: File exceeds 3MB limit.\n" + COLOR_RESET)
            self.log_activity(f"[ERROR] File '{filename}' from user '{client.username}' exceeds size limit ({file_size / (1024.0 * 1024.0):.2f} MB).")
            return
        
        # Check queue status
        queue_position = self.upload_queue.qsize() + 1
        
        if queue_position > MAX_UPLOAD_QUEUE:
            wait_time = (queue_position - MAX_UPLOAD_QUEUE) * 5
            msg = COLOR_YELLOW + f"[Server]: Upload queue is full. Your file will be queued at position {queue_position}. Estimated wait: {wait_time} seconds.\n" + COLOR_RESET
            self.send_message(client.socket, msg)
        else:
            msg = COLOR_GREEN + f"[Server]: File '{filename}' ({self.format_file_size(file_size)}) added to upload queue at position {queue_position}.\n" + COLOR_RESET
            self.send_message(client.socket, msg)
        
        # Add to upload queue
        upload = FileUpload(filename, client.username, target_user, file_size)
        self.upload_queue.put(upload)
        
        self.log_activity(f"[COMMAND] {client.username} initiated file transfer of '{filename}' ({file_size / (1024.0 * 1024.0):.2f} MB) to {target_user}")
        self.log_activity(f"[FILE-QUEUE] Upload '{filename}' from {client.username} added to queue. Queue size: {self.upload_queue.qsize()}")
    
    def file_upload_worker(self):
        self.log_activity("[INFO] File upload worker thread started and waiting for files")
        
        while self.server_running:
            try:
                upload = self.upload_queue.get(timeout=1)
                
                # Calculate wait time
                wait_time = int(time.time() - upload.enqueue_time)
                if wait_time > 0:
                    self.log_activity(f"[FILE] '{upload.filename}' from user '{upload.sender}' started upload after {wait_time} seconds in queue.")
                
                # Notify sender
                sender = self.find_client_by_username(upload.sender)
                if sender and sender.active:
                    msg = COLOR_YELLOW + f"[Server]: Processing file '{upload.filename}' ({self.format_file_size(upload.size)})...\n" + COLOR_RESET
                    self.send_message(sender.socket, msg)
                
                # Notify receiver
                receiver = self.find_client_by_username(upload.receiver)
                if receiver and receiver.active:
                    msg = COLOR_BLUE + f"[Server]: Incoming file from {upload.sender}: '{upload.filename}' ({self.format_file_size(upload.size)})\n" + COLOR_RESET
                    self.send_message(receiver.socket, msg)
                
                # Log transfer start
                self.log_activity(f"[FILE] Starting upload of '{upload.filename}' from {upload.sender} to {upload.receiver} ({self.format_file_size(upload.size)})")
                
                # Simulate transfer with progress
                for i in range(1, 5):
                    time.sleep(1)
                    
                    if sender and sender.active:
                        progress_msg = COLOR_YELLOW + f"[Server]: Upload progress: {i * 25}%\n" + COLOR_RESET
                        self.send_message(sender.socket, progress_msg)
                    
                    if receiver and receiver.active:
                        progress_msg = COLOR_BLUE + f"[Status]: Download progress: {i * 25}%\n"
                        self.send_message(receiver.socket, progress_msg)
                
                # Notify completion
                if sender and sender.active:
                    msg = COLOR_GREEN + f"[Server]: File '{upload.filename}' sent successfully to {upload.receiver}!\n" + COLOR_RESET
                    self.send_message(sender.socket, msg)
                
                if receiver and receiver.active:
                    msg = COLOR_GREEN + f"[Server]: '[Received]' '{upload.filename}' from '{upload.sender}' ({self.format_file_size(upload.size)})\n" + COLOR_RESET
                    self.send_message(receiver.socket, msg)
                    
                    msg = COLOR_GREEN + f"[Server]: File saved as: downloads/{upload.sender}_{upload.filename}\n" + COLOR_RESET
                    self.send_message(receiver.socket, msg)
                
                self.log_activity(f"[SEND FILE] '{upload.filename}' sent from {upload.sender} to {upload.receiver} (success) - {self.format_file_size(upload.size)}")
                
                # Record file as received
                self.add_received_file(upload.receiver, upload.filename, upload.sender)
                
            except queue.Empty:
                continue
            except Exception as e:
                if self.server_running:
                    self.log_activity(f"[ERROR] File upload worker error: {e}")
        
        self.log_activity("[INFO] File upload worker thread exiting")
    
    def handle_exit(self, client):
        self.send_message(client.socket, COLOR_GREEN + "[Server]: Disconnected. Goodbye!\n" + COLOR_RESET)
        print(f"[DISCONNECT] Client {client.username} disconnected.")
        self.log_activity(f"[DISCONNECT] Client {client.username} disconnected.")
    
    def cleanup_client(self, client):
        username = client.username
        
        # Remove from current room
        if client.current_room:
            self.remove_client_from_room(client)
        
        # Remove from clients dict
        with self.clients_lock:
            if client.socket in self.clients:
                del self.clients[client.socket]
        
        # Close socket
        try:
            client.socket.close()
        except:
            pass
        
        client.active = False
        
        if username:
            self.log_activity(f"[DISCONNECT] user '{username}' lost connection. Cleaned up resources.")
    
    def signal_handler(self, sig, frame):
        if sig == signal.SIGINT:
            print("\n[SHUTDOWN] SIGINT received. Shutting down server...")
            
            # Count active clients
            with self.clients_lock:
                active_count = len(self.clients)
            
            self.log_activity(f"[SHUTDOWN] SIGINT received. Disconnecting {active_count} clients, saving logs.")
            
            self.server_running = False
            
            # Notify all clients
            with self.clients_lock:
                for client in self.clients.values():
                    if client.active:
                        self.send_message(client.socket, COLOR_RED + "[Server]: Server is shutting down. Goodbye!\n" + COLOR_RESET)
                        try:
                            client.socket.close()
                        except:
                            pass
            
            # Close server socket
            try:
                self.server_socket.close()
            except:
                pass
            
            sys.exit(0)


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <port>")
        sys.exit(1)
    
    try:
        port = int(sys.argv[1])
    except ValueError:
        print("Invalid port number")
        sys.exit(1)
    
    server = ChatServer(port)
    server.start()


if __name__ == "__main__":
    main()
