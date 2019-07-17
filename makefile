CC=g++
CPPFLAGS=-IC:/SmarAct/SCU/SDK/include
LIBS = -L"C:/SmarAct/SCU/SDK/lib" -lSCU3DControl
OBJ = main.o functions.o icon.o

smaract: $(OBJ)
	$(CC) -o smaract $(OBJ) $(CPPFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJ)