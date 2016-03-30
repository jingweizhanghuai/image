CC = gcc
XX = gcc

CCFLAGS = -O2
XXFLAGS = -O2 

MAINC = $(wildcard *_test.c)
OUTPUT = $(patsubst %.c,%.exe,$(MAINC))

CSOURCES = $(wildcard *.c)
CPPSOURCES = $(wildcard *.cpp)
SSOURCES = $(wildcard *.s)
OBJS = $(patsubst %.c,%.o,$(CPPSOURCES)) $(patsubst %.c,%.o,$(CSOURCES)) $(patsubst %.s,%.o,$(SSOURCES))

LIBS = -ljpeg

$(OUTPUT):$(OBJS)
	$(XX) $(XXFLAGS) $(OBJS) $(LIBS) -o $(OUTPUT) 

%.o:%.c
	$(CC) $(CCFLAGS) -c $< -o $@ 
%.o:%.cpp
	$(XX) $(XXFLAGS) -c $< -o $@

clean:
	del *.o *.exe




