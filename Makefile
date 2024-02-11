SRC_DIR := src
OBJ_DIR := obj
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)
TARGET := ./out

EXTRA_FLAGS ?= -O2

CPPFLAGS := -MMD -MP -std=c++20
CXXFLAGS := -Wall -Wextra -Wpedantic $(EXTRA_FLAGS)
CFLAGS := $(CXXFLAGS)
LDFLAGS :=

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p obj
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean run gdb

clean:
	-rm -rf obj
	-rm -f $(TARGET)

run: $(TARGET)
	$(TARGET)

gdb:
	make clean
	EXTRA_FLAGS=-g make $(TARGET)
	gdb $(TARGET)

-include $(DEPS)
