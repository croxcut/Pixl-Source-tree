CXX = g++
CFLAGS = -g --std=c++17 -static

# Directories
SRC_DIR = src
GLAD_DIR = dependencies/GLAD
GLFW_DIR = dependencies/GLFW
GLM_DIR = dependencies/GLM
BUILD_DIR = builds/windows

# Include paths
INCLUDES = -I$(GLFW_DIR)/include \
           -I$(GLAD_DIR)/include \
           -I$(GLM_DIR)

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/engine/Window.c \
		  $(SRC_DIR)/engine/gfx/Shader.c \
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

# Clean build directory
clean:
	@echo Cleaning...
	@rm -rf $(BUILD_DIR)

.PHONY: all clean