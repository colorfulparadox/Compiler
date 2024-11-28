CXXFLAGS = -pedantic -Wall -std=c++20
SRC_DIR = src
INCLUDES_DIR = includes
BUILD_DIR = build
PROJECT_NAME = out

# A List of src files in the src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# object file names from source file names
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

$(PROJECT_NAME): $(OBJS)
	g++ $(CXXFLAGS) -I $(INCLUDES_DIR) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	g++ $(CXXFLAGS) -I $(INCLUDES_DIR) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(PROJECT_NAME) compile_commands.json


# bear
# https://github.com/rizsotto/Bear
# to generate compile_commands.json
