CC = gcc
XX = gcc

CCFLAGS = -O2 
XXFLAGS = -O2

MAINSOURCE = $(wildcard ./test/fit_test.c)
OUTPUT = $(patsubst %.c,%.exe,$(MAINSOURCE))

CSOURCES = $(wildcard ./src/*.c)
CPPSOURCES = $(wildcard ./src/*.cpp)
SSOURCES = $(wildcard ./src/*.s)
OBJS = $(patsubst %.cpp,%.o,$(CPPSOURCES)) $(patsubst %.c,%.o,$(CSOURCES)) $(patsubst %.s,%.o,$(SSOURCES)) $(patsubst %.c,%.o,$(MAINSOURCE))

INCLUDE_DIR = -I D:\OpenCV2.4.8\opencv\build\include\opencv -I D:\OpenCV2.4.8\opencv\build\include -I .\include

LIBS_DIR = -L D:\OpenCV2.4.8\opencv\build\x86\vc10\lib
LIBS = -ljpeg -lpng -lz -lopencv_core248 -lopencv_imgproc248 -lopencv_highgui248

$(OUTPUT):$(OBJS)
	$(XX) $(XXFLAGS) $(OBJS) $(LIBS_DIR) $(LIBS) -o $(OUTPUT) 

%.o:%.c
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) -c $< -o $@ 
%.o:%.cpp
	$(XX) $(XXFLAGS) $(INCLUDE_DIR) -c $< -o $@

clean:
	del .\src\*.o .\test\*.o .\test\*.exe

