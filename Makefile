# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = -lgtest -lgtest_main -pthread

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests

# Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Targets
APP = $(BIN_DIR)/app
TESTS = $(BIN_DIR)/testScanner $(BIN_DIR)/testRDP

# Default target
all: $(APP)

# Build main application
$(APP): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build and run all tests
test: $(TESTS)

# Definition of tests
$(BIN_DIR)/testScanner: $(BUILD_DIR)/Scanner.o $(BUILD_DIR)/testScanner.test.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/testRDP: $(BUILD_DIR)/Scanner.o $(BUILD_DIR)/RDP.o $(BUILD_DIR)/testRDP.test.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile Test Object Files
$(BUILD_DIR)/testScanner.test.o: $(TEST_DIR)/testScanner.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(BUILD_DIR)/testRDP.test.o: $(TEST_DIR)/testRDP.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean test
