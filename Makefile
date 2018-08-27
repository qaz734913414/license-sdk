CC = gcc 
XX = g++ 
SXX = ar
SFLAGS = crv
CFLAGS = -Wall -std=c++11 -fPIC -g
LIB += -std=c++11 
INC = -I./

TARGET = libtutils.a

%.o: %.c 

	$(CC) $(CFLAGS) $(INC) -c $< -o $@

%.o:%.cpp

	$(XX) $(CFLAGS) $(INC) -c $< -o $@



SOURCES = $(wildcard lib/*.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOURCES))


$(TARGET) : $(OBJS)
	$(SXX) $(SFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm lib/*.o $(TARGET)

echo:
	echo $(SOURCES)
