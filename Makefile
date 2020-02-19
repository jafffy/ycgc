CC=g++ -std=c++14
OUT=encoder
SRC=main.cc pseudo-entropy.cc
LIB=-lx264 -lopencv_core -lopencv_imgcodecs

all:
	$(CC) -o $(OUT) $(SRC) $(LIB)
