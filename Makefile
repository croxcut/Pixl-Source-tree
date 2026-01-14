CXX = g++
CFLAGS = -std=c++17 -O2 -static-libgcc -static-libstdc++

ENGINE_DIR = .
GAME_DIR = game
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
RES_DIR = res
BUILD_RES_DIR = $(BUILD_DIR)/res

GLAD_DIR = dependencies/GLAD
GLFW_DIR = dependencies/GLFW
GLM_DIR = dependencies/GLM
JSON_DIR = dependencies/JSON
ASSIMP_DIR = dependencies/ASSIMP
IMGUI_DIR = dependencies/imgui

INCLUDES = -I$(ENGINE_DIR)\
           -I$(GLFW_DIR)/include \
           -I$(GLAD_DIR)/include \
           -I$(JSON_DIR)/include \
           -I$(ASSIMP_DIR)/include \
           -I$(GLM_DIR) \
           -I$(IMGUI_DIR) \
           -I$(IMGUI_DIR)/backends

ENGINE_SRC = $(wildcard $(ENGINE_DIR)/core/*.cpp) \
             $(wildcard $(ENGINE_DIR)/core/*/*.cpp) \
             $(wildcard $(ENGINE_DIR)/core/*/*/*.cpp) \
             $(wildcard $(ENGINE_DIR)/main/*.cpp) \
             
GAME_SRC = $(wildcard $(GAME_DIR)/*.cpp) \
           $(wildcard $(GAME_DIR)/*/*.cpp)

GLAD_SRC = $(GLAD_DIR)/src/glad.c

IMGUI_SRC = $(IMGUI_DIR)/imgui.cpp \
            $(IMGUI_DIR)/imgui_draw.cpp \
            $(IMGUI_DIR)/imgui_widgets.cpp \
            $(IMGUI_DIR)/imgui_tables.cpp \
            $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
            $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

ENGINE_OBJ = $(patsubst $(ENGINE_DIR)/%.cpp,$(OBJ_DIR)/engine/%.o,$(ENGINE_SRC))
GAME_OBJ   = $(patsubst $(GAME_DIR)/%.cpp,$(OBJ_DIR)/game/%.o,$(GAME_SRC))
GLAD_OBJ   = $(patsubst $(GLAD_DIR)/%.c,$(OBJ_DIR)/glad/%.o,$(GLAD_SRC))
IMGUI_OBJ  = $(IMGUI_SRC:$(IMGUI_DIR)/%.cpp=$(OBJ_DIR)/imgui/%.o)

OBJECTS = $(ENGINE_OBJ) $(GAME_OBJ) $(GLAD_OBJ) $(IMGUI_OBJ)

OUTPUT = $(BUILD_DIR)/Pixl.exe

LIBS = -lopengl32 -L$(GLFW_DIR)/lib-mingw -lglfw3 \
       -lgdi32 -luser32 -lkernel32 -lshell32 \
       -L$(ASSIMP_DIR)/lib -lassimp \
       -limm32

.PHONY: all run clean resources

all: $(OUTPUT)

resources:
	@mkdir -p $(BUILD_RES_DIR)
	cp -r $(RES_DIR)/* $(BUILD_RES_DIR)/

$(OUTPUT): $(OBJECTS) | resources
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(OBJECTS) -o $@ $(LIBS)
	cp $(ASSIMP_DIR)/bin/libassimp-6.dll $(BUILD_DIR)/

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

run: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -rf $(OBJ_DIR) $(BUILD_RES_DIR)
