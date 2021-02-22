.PHONY: all help clean

CC = gcc

TARGET_CLI = main
TARGET_DAEMON = daemon

SRCS_CLI = shell.c binaryTree.c
SRCS_DAEMON = sniffer.c

ALL = $(TARGET_CLI) $(TARGET_DAEMON)

all:	$(ALL)
	@echo Processing target main...
$(TARGET_CLI):	$(TARGET_CLI).c $(SRCS_CLI)
	$(CC) -o $(TARGET_CLI) $(TARGET_CLI).c $(SRCS_CLI)
	./$(TARGET_CLI)
	@echo Target main is running!

$(TARGET_DAEMON):	$(TARGET_DAEMON).c $(SRCS_DAEMON)
	@echo Processing target daemon...
	$(CC) -o $(TARGET_DAEMON) $(TARGET_DAEMON).c $(SRCS_DAEMON)
	./$(TARGET_DAEMON)
	@echo Target daemon is running!

help:
	@echo ------------------------------------------------
	@echo Available options:
	@echo 'make help' - show this message.
	@echo 'make all' - process all targets.
	@echo 'make clean' - delete all files, created by 'make main' and 'make daemon', including log.txt and IPlog.txt.
	@echo 'make daemon' - process target 'daemon'. It starts daemon itself independently in background.
	@echo 'make main' - process target 'main'. It starts daemon, using command line interface.
	@echo ------------------------------------------------
	@echo Be careful about 'log.txt' and 'IPlog.txt' files!
	@echo If you do not stop daemon and remove log files,
	@echo writing information to them  will be continued.
	@echo It will lead to memory leaks.
	@echo ------------------------------------------------

clean:
	@echo Cleaning...
	$(RM) $(ALL) *.txt
	@echo Everything is cleaned!
