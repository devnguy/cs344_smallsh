SRC_DIR := ./src
OBJ_DIR := ./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -Wall -g
CXXFLAGS := -std=gnu99
TARGET := smallsh
LINKER := -lm

$(TARGET): $(OBJ_FILES)
	@echo [LINKING]  All files . . .
	gcc $(LDFLAGS) -o $@ $^ $(LINKER)
	@echo [SUCCESS]  File Name: $(TARGET)
	

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo [BUILDING] $< . . .
	gcc $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(TARGET)