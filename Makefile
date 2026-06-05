CXX := clang++
AR := ar
CLANG_FORMAT ?= clang-format
BACKEND ?= vulkan

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
LIB_DIR := $(BUILD_DIR)/lib

ENGINE_DIR := engine
GAME_DIR := game

ENGINE_SOURCES := $(shell find $(ENGINE_DIR) -name '*.cpp')
ENGINE_HEADERS := $(shell find $(ENGINE_DIR) -name '*.h')
GAME_SOURCES := $(shell find $(GAME_DIR) -name '*.cpp')
GAME_HEADERS := $(shell find $(GAME_DIR) -name '*.h')
FORMAT_FILES := $(ENGINE_HEADERS) $(ENGINE_SOURCES) $(GAME_HEADERS) $(GAME_SOURCES)

ENGINE_OBJECTS := $(ENGINE_SOURCES:%.cpp=$(OBJ_DIR)/%.o)
METAL_GAME_OBJECTS := $(GAME_SOURCES:%.cpp=$(OBJ_DIR)/metal/%.o)
VULKAN_GAME_OBJECTS := $(GAME_SOURCES:%.cpp=$(OBJ_DIR)/vulkan/%.o)
DEPS := $(ENGINE_OBJECTS:.o=.d) $(METAL_GAME_OBJECTS:.o=.d) $(VULKAN_GAME_OBJECTS:.o=.d)

ENGINE_LIB := $(LIB_DIR)/libengine.a
METAL_BIN := $(BIN_DIR)/game-metal
VULKAN_BIN := $(BIN_DIR)/game-vulkan

CPPFLAGS += -I$(ENGINE_DIR)
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic

METAL_LDFLAGS := -framework Metal -framework MetalKit -framework QuartzCore -framework Cocoa

ifdef VULKAN_SDK
VULKAN_CPPFLAGS := -I$(VULKAN_SDK)/include
VULKAN_LDFLAGS := -L$(VULKAN_SDK)/lib -Wl,-rpath,$(VULKAN_SDK)/lib -lvulkan
else
VULKAN_MISSING := 1
endif

.PHONY: all metal vulkan engine format clean

all:
	$(MAKE) $(BACKEND)

engine: format $(ENGINE_LIB)

metal: format $(METAL_BIN)

vulkan: format
ifeq ($(VULKAN_MISSING),1)
	$(error VULKAN_SDK is not set. Install the Vulkan SDK and run `make vulkan VULKAN_SDK=/path/to/VulkanSDK`)
endif
	$(MAKE) $(VULKAN_BIN)

format:
	$(CLANG_FORMAT) -i $(FORMAT_FILES)

$(METAL_BIN): $(METAL_GAME_OBJECTS) $(ENGINE_LIB)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(METAL_LDFLAGS)

$(VULKAN_BIN): $(VULKAN_GAME_OBJECTS) $(ENGINE_LIB)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(VULKAN_LDFLAGS)

$(ENGINE_LIB): $(ENGINE_OBJECTS)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/metal/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/vulkan/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(VULKAN_CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
