CPP_FILES := $(wildcard src/*.cpp Monster/*.cpp Player/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

INC_PATH = -Iinc -IMonster -IPlayer

#GNU C/C++ Compiler
GCC = g++

# GNU C/C++ Linker
LINK = g++

# Compiler flags
CFLAGS = -Wall -O -std=c++11
CXXFLAGS = $(CFLAGS)

# Fill in special libraries needed here
LIBS = -lglut -lGLU -lGL -lm -lpthread

.PHONY: clean

VPATH = src:Monster:Player

# Targets include all, clean, debug, tar

all : main

main: $(OBJ_FILES)
	$(LINK) -o $@ $^ $(LIBS)

obj/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(GCC) $(CXXFLAGS) $(INC_PATH) -c -o $@ $^

obj/%.o: Monster/%.cpp
	@mkdir -p $(@D)
	$(GCC) $(CXXFLAGS) $(INC_PATH) -c -o $@ $^

obj/%.o: Player/%.cpp
	@mkdir -p $(@D)
	$(GCC) $(CXXFLAGS) $(INC_PATH) -c -o $@ $^

clean:
	rm -rf core main obj/*.o

debug: CXXFLAGS = -DDEBUG -g -std=c++11
debug: main
