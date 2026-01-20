# Compiler and flags
CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -Wextra -DNDEBUG -static-libgcc -static-libstdc++

# Directories
ENGINE_DIR = .
GAME_DIR = game
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
RES_DIR = res
BUILD_RES_DIR = $(BUILD_DIR)/res

# Dependencies
GLAD_DIR = dependencies/GLAD
GLFW_DIR = dependencies/GLFW
GLM_DIR = dependencies/GLM
JSON_DIR = dependencies/JSON
ASSIMP_DIR = dependencies/ASSIMP
IMGUI_DIR = dependencies/imgui

# Include paths
INCLUDES = -I$(ENGINE_DIR) \
           -I$(GLFW_DIR)/include \
           -I$(GLAD_DIR)/include \
           -I$(JSON_DIR)/include \
           -I$(ASSIMP_DIR)/include \
           -I$(GLM_DIR) \
           -I$(IMGUI_DIR) \
           -I$(IMGUI_DIR)/backends

# Source files
ENGINE_SRC = \
	$(wildcard $(ENGINE_DIR)/core/*.cpp) \
	$(wildcard $(ENGINE_DIR)/core/*/*.cpp) \
	$(wildcard $(ENGINE_DIR)/core/*/*/*.cpp) \
	$(wildcard $(ENGINE_DIR)/main/*.cpp)
GAME_SRC   = $(wildcard $(GAME_DIR)/*.cpp)
GLAD_SRC   = $(GLAD_DIR)/src/glad.c
IMGUI_SRC = \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# Object files
ENGINE_OBJ = $(patsubst $(ENGINE_DIR)/%.cpp,$(OBJ_DIR)/engine/%.o,$(ENGINE_SRC))
GAME_OBJ   = $(patsubst $(GAME_DIR)/%.cpp,$(OBJ_DIR)/game/%.o,$(GAME_SRC))
GLAD_OBJ   = $(patsubst $(GLAD_DIR)/%.c,$(OBJ_DIR)/glad/%.o,$(GLAD_SRC))
IMGUI_OBJ  = $(patsubst $(IMGUI_DIR)/%.cpp,$(OBJ_DIR)/imgui/%.o,$(IMGUI_SRC))

OBJECTS = $(ENGINE_OBJ) $(GAME_OBJ) $(GLAD_OBJ) $(IMGUI_OBJ)
OUTPUT = $(BUILD_DIR)/Pixl.exe

# Libraries
LIBS = -lopengl32 -L$(GLFW_DIR)/lib-mingw -lglfw3 \
       -lgdi32 -luser32 -lkernel32 -lshell32 \
       -L$(ASSIMP_DIR)/lib -lassimp \
       -limm32

# Phony targets
.PHONY: all run clean resources

# Default target
all: $(OUTPUT)

# Copy resources
resources:
	@mkdir -p $(BUILD_RES_DIR)
	cp -r $(RES_DIR)/* $(BUILD_RES_DIR)/ || true

# Build executable
$(OUTPUT): $(OBJECTS) | resources
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(OBJECTS) -o $@ $(LIBS)
	cp -u $(ASSIMP_DIR)/bin/libassimp-6.dll $(BUILD_DIR)/ || true

# Pattern rules for object files
$(OBJ_DIR)/engine/%.o: $(ENGINE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/game/%.o: $(GAME_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/glad/%.o: $(GLAD_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/imgui/%.o: $(IMGUI_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run the program
run: $(OUTPUT)
	@echo "Running $(OUTPUT)..."
	./$(OUTPUT)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BUILD_RES_DIR) $(OUTPUT)