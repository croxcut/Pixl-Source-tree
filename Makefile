CXX = g++
CFLAGS = -g --std=c++17 -static -O2

# Directories
SRC_DIR = pixl/src
GLAD_DIR = dependencies/GLAD
GLFW_DIR = dependencies/GLFW
GLM_DIR = dependencies/GLM
BUILD_DIR = builds
GAME_DIR = game

# Include paths
INCLUDES = -I$(GLFW_DIR)/include \
           -I$(GLAD_DIR)/include \
           -I$(GLM_DIR)

# Source files
SOURCES = $(GAME_DIR)/main.cpp \
          $(SRC_DIR)/engine/Window.cpp \
          $(SRC_DIR)/engine/gfx/renderer/*.cpp \
          $(SRC_DIR)/engine/gfx/renderer/opengl/*.cpp \
          $(GLAD_DIR)/src/glad.c 

# Output binary
OUTPUT = $(BUILD_DIR)/main.exe

# Linker libraries
LIBS = -lopengl32 -L$(GLFW_DIR)/lib-mingw -lglfw3dll

# Default build target
all: $(OUTPUT)

# Run Target :)
run: all
	./builds/windows/main.exe

$(OUTPUT): $(SOURCES)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

.PHONY: all clean