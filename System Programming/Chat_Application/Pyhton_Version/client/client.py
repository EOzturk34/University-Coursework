#!/usr/bin/env python3

import socket
import threading
import sys
import signal
import time

# Constants
MAX_MESSAGE_LEN = 1024
MAX_USERNAME_LEN = 16

# ANSI color codes
COLOR_RED = "\033[31m"
COLOR_GREEN = "\033[32m"
COLOR_YELLOW = "\033[33m"
COLOR_BLUE = "\033[34m"
COLOR_RESET = "\033[0m"


class ChatClient:
    def __init__(self, server_ip, port):
        self.server_ip = server_ip
        self.port = port
        self.client_socket = None
        self.running = True
        
        # Set up signal handler
        signal.signal(signal.SIGINT, self.signal_handler)
        signal.signal(signal.SIGTERM, self.signal_handler)
    
    def connect(self):
        try:
            # Create socket
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            
            print(f"Connecting to {self.server_ip}:{self.port}...")
            
            # Connect to server
            self.client_socket.connect((self.server_ip, self.port))
            
            print(COLOR_GREEN + "Connected to server!\n" + COLOR_RESET)
            
            # Start receiver thread
            receiver_thread = threading.Thread(target=self.receive_messages)
            receiver_thread.daemon = True
            receiver_thread.start()
            
            # Main input loop
            self.input_loop()
            
        except ConnectionRefusedError:
            print("Connection failed")
            sys.exit(1)
        except Exception as e:
            print(f"Error: {e}")
            sys.exit(1)
        finally:
            if self.client_socket:
                self.client_socket.close()
    
    def receive_messages(self):
        while self.running:
            try:
                data = self.client_socket.recv(MAX_MESSAGE_LEN)
                if not data:
                    if self.running:
                        print(COLOR_RED + "\nDisconnected from server\n" + COLOR_RESET)
                        self.running = False
                    break
                
                message = data.decode('utf-8')
                print(message, end='', flush=True)
                
            except socket.error:
                if self.running:
                    print(COLOR_RED + "\nDisconnected from server\n" + COLOR_RESET)
                    self.running = False
                break
            except Exception:
                break
    
    def input_loop(self):
        while self.running:
            try:
                # Read input
                user_input = input()
                
                if not self.running:
                    break
                
                # Send to server
                try:
                    self.client_socket.send(user_input.encode('utf-8'))
                except:
                    print("Send failed")
                    break
                
                # Check for exit command
                if user_input.strip() == '/exit':
                    self.running = False
                    break
                    
            except EOFError:
                # Handle Ctrl+D
                break
            except KeyboardInterrupt:
                # Handle Ctrl+C
                break
            except Exception:
                break
    
    def signal_handler(self, sig, frame):
        if sig == signal.SIGINT or sig == signal.SIGTERM:
            print("\n" + COLOR_YELLOW + "Disconnecting...\n" + COLOR_RESET)
            self.running = False
            
            if self.client_socket:
                try:
                    self.client_socket.send(b'/exit')
                    self.client_socket.close()
                except:
                    pass
            
            sys.exit(0)


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <server_ip> <port>")
        sys.exit(1)
    
    server_ip = sys.argv[1]
    
    try:
        port = int(sys.argv[2])
    except ValueError:
        print("Invalid port number")
        sys.exit(1)
    
    client = ChatClient(server_ip, port)
    client.connect()


if __name__ == "__main__":
    main()