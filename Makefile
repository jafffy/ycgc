CC=g++ -std=c++14

ENC_OUT=encoder
ENC_SRC=encoder_main.cc pseudo-entropy.cc
ENC_INC=-I../x264
ENC_LIB=-L../x264 -lx264 -lopencv_core -lopencv_imgcodecs -ldl -lpthread

DEC_OUT=decoder
DEC_SRC=decoder_main.cc
DEC_INC=-I../x264
DEC_LIB=-L../x264 -lx264 -lopencv_core -lopencv_imgcodecs -ldl -lpthread

encoder:
	$(CC) -o $(ENC_OUT) $(ENC_SRC) $(ENC_LIB) $(ENC_INC)

decoder:
	$(CC) -o $(DEC_OUT) $(DEC_SRC) $(DEC_LIB) $(DEC_INC)
