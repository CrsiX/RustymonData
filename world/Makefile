# Name of the output executable
OUT_EXE = world_generator

# Name of the input C++ file
IN_CPP = world_generator.cpp

all: clean build

build:
	g++ ${IN_CPP} -o ${OUT_EXE} --std=c++14 -lpthread -lz -lexpat -lbz2 -ljsoncpp

clean:
	rm -rvf ${OUT_EXE}

install:
	@echo "There's no target 'install'. Use 'build' or 'clean' instead."
