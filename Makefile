# Compiler and flags
CXX = g++
CXXFLAGS = -Isrc/include -Wall -DCHESSBOARD_CPP_INCLUDED -D_HAS_STD_BYTE=0 -std=c++14
LDFLAGS = -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lopengl32

# Source files explicitly listed for better control
SRC = coding/main.cpp \
      coding/ChessBoard.cpp \
      coding/ChessBoardMenu.cpp \
      coding/ChessBoardEngine.cpp \
      coding/ChessBoardNetwork.cpp \
      coding/GameLogic.cpp \
      coding/StockfishEngine.cpp \
      coding/NetworkManager.cpp

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