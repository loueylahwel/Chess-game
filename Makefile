# Compiler and flags
CXX = g++
CXXFLAGS = -Isrc/include -Wall
LDFLAGS = -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lopengl32

SRC = $(wildcard coding/*.cpp)
TARGET = main.exe

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	@echo "Removing old executable..."
	@rm -f $(TARGET)
	@echo "Compiling new executable..."
	$(CXX) -o $(TARGET) $(SRC) $(CXXFLAGS) $(LDFLAGS)
	@echo "Build complete!"

clean:
	@echo "Cleaning up..."
	@rm -f $(TARGET)
	@rm -f *.o
	@rm -f src/*.o
	@echo "Clean complete!"