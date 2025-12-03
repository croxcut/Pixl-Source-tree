# Compiler and flags
CXX = g++
CFLAGS = -g -std=c++17 -O0

# Directories
ENGINE_DIR = pixl
GAME_DIR = game
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Dependencies
GLAD_DIR = dependencies/GLAD
GLFW_DIR = dependencies/GLFW
GLM_DIR = dependencies/GLM

# Includes
INCLUDES = -I$(ENGINE_DIR)/include \
           -I$(GLFW_DIR)/include \
           -I$(GLAD_DIR)/include \
           -I$(GLM_DIR)

# Source files
ENGINE_SRC = $(wildcard $(ENGINE_DIR)/src/*.cpp) \
             $(wildcard $(ENGINE_DIR)/src/*/*.cpp) \
             $(wildcard $(ENGINE_DIR)/src/*/*/*.cpp) \
             $(wildcard $(ENGINE_DIR)/src/*/*/*/*.cpp)

GAME_SRC = $(wildcard $(GAME_DIR)/src/*.cpp)
GLAD_SRC = $(GLAD_DIR)/src/glad.c

# Object files
ENGINE_OBJ = $(patsubst $(ENGINE_DIR)/%.cpp,$(OBJ_DIR)/engine/%.o,$(ENGINE_SRC))
GAME_OBJ = $(patsubst $(GAME_DIR)/%.cpp,$(OBJ_DIR)/game/%.o,$(GAME_SRC))
GLAD_OBJ = $(patsubst $(GLAD_DIR)/%.c,$(OBJ_DIR)/glad/%.o,$(GLAD_SRC))

OBJECTS = $(ENGINE_OBJ) $(GAME_OBJ) $(GLAD_OBJ)

# Output binary
OUTPUT = $(BUILD_DIR)/main.exe

# Libraries
LIBS = -lopengl32 -L$(GLFW_DIR)/lib-mingw -lglfw3 -lgdi32 -luser32 -lkernel32 -lshell32

# Default build
all: $(OUTPUT)

# Link final executable
$(OUTPUT): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $^ -o $@ $(LIBS)

# Compile C++ engine files
$(OBJ_DIR)/engine/%.o: $(ENGINE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile C++ game files
$(OBJ_DIR)/game/%.o: $(GAME_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile C GLAD files
$(OBJ_DIR)/glad/%.o: $(GLAD_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run the program
run: $(OUTPUT)
	./$(OUTPUT)

# Clean build
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean