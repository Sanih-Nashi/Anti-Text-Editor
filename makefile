
CC = g++
CFLAGS =-fsanitize=address -fdiagnostics-color=always -O3 -std=c++23
# Source files (find all .cpp files recursively from the src directory)
SOURCES = $(shell find src -name "*.cpp")

# Output executable
OUTPUTFILE = anti

# Default target
all: 
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUTFILE)

# Clean target (delete object files and executable)

clean:
	rm -f $(OUTPUTFILE) *.o
# Phony targets (not related to real files)
.PHONY: all clean
