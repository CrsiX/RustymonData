# Name of the output executable
OUT_EXE = PokeWorldGenerator

# Name of the input C++ file
IN_CPP = PokeWorldGenerator.cpp

all: clean build run

build:
	g++ ${IN_CPP} -o ${OUT_EXE} --std=c++14 -lpthread -lz -lexpat -lbz2 -ljsoncpp

clean:
	rm -rvf ${OUT_EXE}

run:
	./${OUT_EXE} ${INFILE}

install:
	@echo "There's no target 'install'. Use 'build' or 'clean' instead."
