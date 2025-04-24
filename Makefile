# Compiler and flags
CXX = g++
CXXFLAGS = -Isrc/include -Wall
LDFLAGS = -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lopengl32

# Source files (in chess/coding/) and target (in chess/)
SRC = $(wildcard coding/*.cpp)
TARGET = main.exe

.PHONY: all clean

# Default target
all: $(TARGET)

# Build rule - removes old .exe first, then compiles
$(TARGET): $(SRC)
	@echo "Removing old $(TARGET) if it exists..."
	@rm -f $(TARGET)
	@echo "Compiling new executable..."
	$(CXX) -o $(TARGET) $(SRC) $(CXXFLAGS) $(LDFLAGS)
	@echo "Build complete! $(TARGET) created."

# Clean rule (removes .exe and object files)
clean:
	@echo "Cleaning up..."
	@rm -f $(TARGET)
	@rm -f *.o
	@rm -f coding/*.o
	@echo "Clean complete."