EXEC_NAME  := generate_image

CCX        := g++
CXXFLAGS   :=
SRC_FILES  := $(wildcard ./*.cpp)
OBJ_FILES  := $(SRC_FILES:.cpp=.o)

.PHONY: all clean test

$(EXEC_NAME): $(OBJ_FILES)
	$(CCX) $(CXXFLAGS) $^ -o $(EXEC_NAME)

$(OBJ_FILES): $(SRC_FILES)

$(SRC_FILES):
	$(CCX) -c $(CXXFLAGS) $@

clean:
	rm -rf $(OBJ_FILES)
	rm -rf $(EXEC_NAME)
