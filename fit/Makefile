CC = gcc
XX = gcc
RM = del

CCFLAGS = -O2 
XXFLAGS = -O2

MAINSOURCE = $(wildcard ./test/fit_test.c)
OUTPUT = $(patsubst %.c,%.exe,$(MAINSOURCE))

CSOURCES = $(wildcard ./src/*.c)
CPPSOURCES = $(wildcard ./src/*.cpp)
SSOURCES = $(wildcard ./src/*.s)
OBJS = $(patsubst %.cpp,%.o,$(CPPSOURCES)) $(patsubst %.c,%.o,$(CSOURCES)) $(patsubst %.s,%.o,$(SSOURCES)) $(patsubst %.c,%.o,$(MAINSOURCE))

INCLUDE_DIR = -I .\include
LIBS = -ljpeg -lpng -lz

$(OUTPUT):$(OBJS)
	$(XX) $(XXFLAGS) $(OBJS) $(LIBS_DIR) -o $(OUTPUT) 

%.o:%.c
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) -c $< -o $@ 
%.o:%.cpp
	$(XX) $(XXFLAGS) $(INCLUDE_DIR) -c $< -o $@

clean:
	$(RM) .\src\*.o .\test\*.o .\test\*.exe

