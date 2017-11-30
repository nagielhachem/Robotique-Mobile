# Setting GPU compiler and flags
CPP = g++ 
CPPFLAGS= --std=c++11 

CPPLIBS = 

##############################################################################

# Sources

# Setting library files and dir
LDIR = lib
_LIB = 

LIB = $(parsubst %, $(LDIR)/%, $(_LIB))

# Setting src files and dir
SDIR = src
_SRC = simpleMotionCommands.cpp

SRC = $(patsubst %, $(SDIR)/%, $(_SRC))

##############################################################################

# Objects

ODIR = obj
_OBJ = $(_SRC:.cpp=.o)
_OBJ += $(_LIB:.cpp=.o)
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

##############################################################################

BIN = simulation

# link
all: $(OBJ)
	$(CPP) $(CPPFLAGS) $(OBJ) $(CPPLIBS) -o $(BIN)

##############################################################################
											# Comiple individually
			######################################################
										# Source code compiling

$(ODIR)/simpleMotionCommands.o: $(SDIR)/simpleMotionCommands.cpp 
	$(CPP) $(CPPFLAGS) -c $< -o $@

			######################################################
											# library compiling

##############################################################################

run: build
	$(EXEC) ./$(BIN)

clean:
	$(RM) $(OBJ) $(BIN) 

.PHONY: clean

##############################################################################
