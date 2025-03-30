# # all:
# # 	g++ -std=c++11 -Wall -O0 -g -lSDL2  -I/opt/homebrew/opt/sdl2/include -L/opt/homebrew/opt/sdl2/lib ./main.cpp -o p
# # all:
# # 	g++ -std=c++11 -Wall -O0 -g -lSDL2 -I/opt/homebrew/Cellar/sdl2/2.32.2/include -I/opt/homebrew/Cellar/sdl2_ttf/2.24.0/include -L/opt/homebrew/Cellar/sdl2/2.32.2/lib -L/opt/homebrew/opt/sdl2_ttf/lib main.cpp GameLogic.cpp Renderer.cpp -o mario

# # clean:
# # 	rm -f mario
# all: 
# # 	g++ -std=c++11 -Wall -O0 -g -lSDL2_ttf -lSDL2 -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2 -lSDL2_ttf main.cpp GameLogic.cpp Renderer.cpp -o mario
CXX = clang++
CXXFLAGS = -std=c++11 -I/opt/homebrew/include -I/opt/homebrew/Cellar/sdl2/2.32.2/include -I/opt/homebrew/Cellar/sdl2_image/2.8.8/include -I/opt/homebrew/Cellar/sdl2_ttf/2.24.0/include
LDFLAGS = -L/opt/homebrew/lib -L/opt/homebrew/Cellar/sdl2/2.32.2/lib -L/opt/homebrew/Cellar/sdl2_image/2.8.8/lib -L/opt/homebrew/Cellar/sdl2_ttf/2.24.0/lib -lSDL2 -lSDL2_ttf -lSDL2_image

TARGET = game
SRC = main.cpp GameLogic.cpp Renderer.cpp Control.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

# CXX = clang++
# CXXFLAGS = -std=c++11 -Wall -I/opt/homebrew/include
# LDFLAGS = -L/opt/homebrew/lib -lSDL2 -lSDL2_ttf -framework Cocoa

# TARGET = game
# SRC = main.cpp GameLogic.cpp Renderer.cpp Control.cpp

# all: $(TARGET)

# $(TARGET): $(SRC)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# clean:
# 	rm -f $(TARGET)