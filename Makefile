USE_HALIDE := True
HALIDE_PATH := /Users/dillon/CppWorkspace/Halide/
HALIDE_BIN_PATH := $(HALIDE_PATH)/bin
HALIDE_INCLUDE_PATH := $(HALIDE_PATH)/include

CXX := /Users/dillon/Downloads/clang+llvm-9.0.0-x86_64-darwin-apple/bin/clang++
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
LDFLAGS :=
CPPFLAGS := 
CXXFLAGS := -std=c++17

ifeq ($(USE_HALIDE), True)
 CXXFLAGS += -D__USE_HALIDE__	-I$(HALIDE_INCLUDE_PATH)
 LDFLAGS += -L$(HALIDE_BIN_PATH) -lHalide -lpthread -ldl
endif

kcamera: $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	g++ $(LDFLAGS) -o $(BIN_DIR)/$@ $^

clean:
	\rm -rf $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
