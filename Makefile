SILENCE=@
GXX=g++
GCC=gcc

#Add include folders here
INC=Architecture/ \
	Architecture/Controllers \
	Architecture/DataStructures \
	Architecture/Event \
	Architecture/HardwareAbstractions \
	Architecture/Timer \
	Architecture/TimerModule \
	Architecture/Utilities \
	Test \
	Test/Doubles \
	
#Add C++ source files here
CPP_SRC=Test/main.cpp \
		Test/Controllers/*.cpp \
		Test/DataStructures/*.cpp \
		Test/Doubles/*.cpp \
		Test/HardwareAbstractions/*.cpp \
		Test/Timer/*.cpp

#Add C source files here
C_SRC=Architecture/Controllers/DistanceProviderCm.c \
	Architecture/Controllers/PathFinder_AStar.c \
	Architecture/Controllers/ScoutingController.c \
	Architecture/Controllers/WayPointProvider_Unscouted.c \
	Architecture/DataStructures/*.c \
	Architecture/Event/*.c \
	Architecture/HardwareAbstractions/*.c \
	Architecture/Timer/*.c \
	Architecture/TimerModule/*.c

INC_PARAMS=$(foreach d, $(INC), -I$d)
EXEC=tests
LIB=-lCppUTest -lCppUTestExt

BIN_FOLDER=bin
	
all:
	$(SILENCE) $(GCC) -std=c99 -c $(C_SRC) $(INC_PARAMS)
	$(SILENCE) $(GXX) -std=c++0x -c $(CPP_SRC) $(INC_PARAMS)
	$(SILENCE) if [ ! -d "$(BIN_FOLDER)" ]; then mkdir $(BIN_FOLDER); fi
	$(SILENCE) mv *.o $(BIN_FOLDER)/
	$(SILENCE) $(GXX) -o $(EXEC) $(BIN_FOLDER)/*.o $(LIB)
	$(SILENCE) ./$(EXEC)
	
clean:
	$(SILENCE)rm -f $(EXEC)
	$(SILENCE)rm -rf $(BIN_FOLDER)