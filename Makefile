# Compiler and flags
CXX = g++
CXXFLAGS := -Wall -Iinclude
LDFLAGS = -lgtest -lgtest_main -pthread

#For Memory Leak Detection: -fsanitize=address
#Conditional compiler flags
BUILD ?= release
ifeq ($(BUILD),debug)
  CXXFLAGS += -g --std=c++14
else ifeq ($(BUILD),release)
  CXXFLAGS += --std=c++14
else
  $(error Error: Please specify a build mode using 'make BUILD=debug' from debug,release,assignment)
endif

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
TESTS = $(BIN_DIR)/testScanner $(BIN_DIR)/testRDP $(BIN_DIR)/testSymTbl $(BIN_DIR)/testTACGen

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
	@./tests/runAllTests.sh

# Definition of tests
$(BIN_DIR)/testScanner: $(BUILD_DIR)/Scanner.o $(BUILD_DIR)/testScanner.test.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/testRDP: $(BUILD_DIR)/Scanner.o $(BUILD_DIR)/RDP.o $(BUILD_DIR)/SymTbl.o $(BUILD_DIR)/TACGen.o $(BUILD_DIR)/testRDP.test.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/testSymTbl: $(BUILD_DIR)/SymTbl.o $(BUILD_DIR)/testSymTbl.test.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/testTACGen: $(BUILD_DIR)/TACGen.o $(BUILD_DIR)/testTACGen.test.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile Test Object Files
$(BUILD_DIR)/testScanner.test.o: $(TEST_DIR)/testScanner.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(BUILD_DIR)/testRDP.test.o: $(TEST_DIR)/testRDP.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(BUILD_DIR)/testSymTbl.test.o: $(TEST_DIR)/testSymTbl.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(BUILD_DIR)/testTACGen.test.o: $(TEST_DIR)/testTACGen.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean test
