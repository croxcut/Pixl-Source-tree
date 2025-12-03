CXX = g++
CFLAGS = -g -std=c++17 -O2

# Directories
ENGINE_DIR = pixl
GAME_DIR = game
BUILD_DIR = game

# Dependencies
GLAD_DIR = dependencies/GLAD
GLFW_DIR = dependencies/GLFW
GLM_DIR = dependencies/GLM

# Includes
INCLUDES = -I$(ENGINE_DIR)/include \
           -I$(GLFW_DIR)/include \
           -I$(GLAD_DIR)/include \
           -I$(GLM_DIR)

# Source files (wildcards)
ENGINE_SRC = $(wildcard $(ENGINE_DIR)/src/*.cpp) \
             $(wildcard $(ENGINE_DIR)/src/*/*.cpp) \
             $(wildcard $(ENGINE_DIR)/src/*/*/*.cpp) \
             $(wildcard $(ENGINE_DIR)/src/*/*/*/*.cpp) \

GAME_SRC = $(wildcard $(GAME_DIR)/src/*.cpp)

GLAD_SRC = $(GLAD_DIR)/src/glad.c

SOURCES = $(ENGINE_SRC) $(GAME_SRC) $(GLAD_SRC)

# Output binary
OUTPUT = $(BUILD_DIR)/main.exe

# Libraries
LIBS = -lopengl32 -L$(GLFW_DIR)/lib-mingw -lglfw3 -lgdi32 -luser32 -lkernel32 -lshell32

# Default build
all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	@mkdir $(BUILD_DIR) 2>nul || true
	$(CXX) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

# Run the program
run: all
	$(OUTPUT)

# Clean build
clean:
	rm -rf $(BUILD_DIR)/main.exe
