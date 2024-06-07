.SECONDARY:
.DELETE_ON_ERROR:

INSTALL_DIR=$(HOME)/.local
INCLUDE=$(INSTALL_DIR)/include
LIB=$(INSTALL_DIR)/lib

all: possel_train 

possel_train : possel_train.cpp negsel_common.o
	g++ -o $@ -std=c++1z -I $(INCLUDE) -L $(LIB) -Wl,-rpath,$(LIB) $^ -lfsacs -lfst

%.o: %.cpp
	g++ -std=c++1z -O2 -DNDEBUG -I $(INCLUDE) -I . -c $^ 

clean :
	rm possel_train negsel_common.o


