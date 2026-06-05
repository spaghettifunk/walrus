ifeq ($(origin CXX),default)
CXX := clang++
endif

CLANG_FORMAT ?= clang-format
BACKEND ?= vulkan
UNAME_S := $(shell uname -s)

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
LIB_DIR := $(BUILD_DIR)/lib

ENGINE_DIR := engine
GAME_DIR := game

ENTRYPOINT_SOURCE := $(ENGINE_DIR)/EntryPoint.cpp
ENGINE_SOURCES := $(shell find $(ENGINE_DIR) -name '*.cpp' ! -name 'EntryPoint.cpp')
ALL_ENGINE_SOURCES := $(ENGINE_SOURCES) $(ENTRYPOINT_SOURCE)
ENGINE_HEADERS := $(shell find $(ENGINE_DIR) -name '*.h')
GAME_SOURCES := $(shell find $(GAME_DIR) -name '*.cpp')
GAME_HEADERS := $(shell find $(GAME_DIR) -name '*.h')
FORMAT_FILES := $(ENGINE_HEADERS) $(ALL_ENGINE_SOURCES) $(GAME_HEADERS) $(GAME_SOURCES)

ENGINE_OBJECTS := $(ENGINE_SOURCES:%.cpp=$(OBJ_DIR)/engine-lib/%.o)
ENTRYPOINT_OBJECT := $(ENTRYPOINT_SOURCE:%.cpp=$(OBJ_DIR)/entrypoint/%.o)
METAL_GAME_OBJECTS := $(GAME_SOURCES:%.cpp=$(OBJ_DIR)/metal/%.o)
VULKAN_GAME_OBJECTS := $(GAME_SOURCES:%.cpp=$(OBJ_DIR)/vulkan/%.o)
DEPS := $(ENGINE_OBJECTS:.o=.d) $(ENTRYPOINT_OBJECT:.o=.d) $(METAL_GAME_OBJECTS:.o=.d) $(VULKAN_GAME_OBJECTS:.o=.d)

ifeq ($(UNAME_S),Darwin)
SHARED_LIB_EXT := dylib
ENGINE_SONAME := libwalrus.$(SHARED_LIB_EXT)
ENGINE_SHARED_LDFLAGS := -dynamiclib -install_name @rpath/$(ENGINE_SONAME)
GAME_RPATH_LDFLAGS := -Wl,-rpath,@executable_path/../lib
else
SHARED_LIB_EXT := so
ENGINE_SONAME := libwalrus.$(SHARED_LIB_EXT)
ENGINE_SHARED_LDFLAGS := -shared -Wl,-soname,$(ENGINE_SONAME)
GAME_RPATH_LDFLAGS := -Wl,-rpath,'$$ORIGIN/../lib'
endif

ENGINE_LIB := $(LIB_DIR)/$(ENGINE_SONAME)
METAL_BIN := $(BIN_DIR)/game-metal
VULKAN_BIN := $(BIN_DIR)/game-vulkan

CPPFLAGS += -I$(ENGINE_DIR)
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic
VISIBILITY_FLAGS := -fvisibility=hidden -fvisibility-inlines-hidden
ENGINE_CPPFLAGS := $(CPPFLAGS) -DWALRUS_EXPORT
ENGINE_CXXFLAGS := $(CXXFLAGS) $(VISIBILITY_FLAGS) -fPIC
GAME_CXXFLAGS := $(CXXFLAGS) $(VISIBILITY_FLAGS)
ENGINE_LDFLAGS := -L$(LIB_DIR) -lwalrus $(GAME_RPATH_LDFLAGS)

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

$(METAL_BIN): $(METAL_GAME_OBJECTS) $(ENTRYPOINT_OBJECT) $(ENGINE_LIB)
	@mkdir -p $(@D)
	$(CXX) $(GAME_CXXFLAGS) $(METAL_GAME_OBJECTS) $(ENTRYPOINT_OBJECT) $(ENGINE_LDFLAGS) -o $@ $(METAL_LDFLAGS)

$(VULKAN_BIN): $(VULKAN_GAME_OBJECTS) $(ENTRYPOINT_OBJECT) $(ENGINE_LIB)
	@mkdir -p $(@D)
	$(CXX) $(GAME_CXXFLAGS) $(VULKAN_GAME_OBJECTS) $(ENTRYPOINT_OBJECT) $(ENGINE_LDFLAGS) -o $@ $(VULKAN_LDFLAGS)

$(ENGINE_LIB): $(ENGINE_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(ENGINE_CXXFLAGS) $(ENGINE_SHARED_LDFLAGS) $^ -o $@

$(OBJ_DIR)/engine-lib/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(ENGINE_CPPFLAGS) $(ENGINE_CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/entrypoint/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(GAME_CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/metal/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(GAME_CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/vulkan/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(VULKAN_CPPFLAGS) $(GAME_CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
