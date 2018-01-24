SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
LDFLAGS :=
CPPFLAGS := 
CXXFLAGS := -std=c++11

kcamera: $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	g++ $(LDFLAGS) -o $(BIN_DIR)/$@ $^

clean:
	\rm -rf $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
