CXXFLAGS+= -Wall -O2 -Wno-deprecated
LDFLAGS+= 

#SDL
#LDFLAGS+= -lSDL -lSDL_gfx

#freenect
LDFLAGS+= -lfreenect

#ni
CXXFLAGS+= -I/usr/include/ni
LDFLAGS+= -lOpenNI

#VTK
CXXFLAGS += -I/usr/include/vtk-5.4
LDFLAGS  += -lvtkRendering

SRC=\
	main.cpp

OBJS=$(SRC:.cpp=.o)
TARGET=kinect

CC = g++ $(CXXFLAGS)

all: $(TARGET)

clean:
	$(RM) $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)
