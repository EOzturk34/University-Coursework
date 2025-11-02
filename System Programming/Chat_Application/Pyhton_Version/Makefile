# Makefile for Python Chat Server and Client

all: chatserver chatclient

chatserver:
	@echo '#!/bin/bash' > chatserver
	@echo 'if command -v python3 &> /dev/null; then' >> chatserver
	@echo '    python3 server/server.py $$@' >> chatserver
	@echo 'elif command -v python &> /dev/null; then' >> chatserver
	@echo '    python server/server.py $$@' >> chatserver
	@echo 'else' >> chatserver
	@echo '    echo "Python is not installed. Please install Python 3."' >> chatserver
	@echo '    exit 1' >> chatserver
	@echo 'fi' >> chatserver
	@chmod +x chatserver
	@echo "Created chatserver executable"

chatclient:
	@echo '#!/bin/bash' > chatclient
	@echo 'if command -v python3 &> /dev/null; then' >> chatclient
	@echo '    python3 client/client.py $$@' >> chatclient
	@echo 'elif command -v python &> /dev/null; then' >> chatclient
	@echo '    python client/client.py $$@' >> chatclient
	@echo 'else' >> chatclient
	@echo '    echo "Python is not installed. Please install Python 3."' >> chatclient
	@echo '    exit 1' >> chatclient
	@echo 'fi' >> chatclient
	@chmod +x chatclient
	@echo "Created chatclient executable"

clean:
	rm -f chatserver chatclient
	rm -f server.log
	rm -f *.pyc __pycache__

.PHONY: all clean
