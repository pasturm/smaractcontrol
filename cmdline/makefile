CC=g++
CPPFLAGS=-IC:/SmarAct/SCU/SDK/include
LIBS = -L"C:/SmarAct/SCU/SDK/lib" -lSCU3DControl
OBJ = smaract_cmdline.o

smaract_cmdline: $(OBJ)
	$(CC) -o smaract_cmdline $(OBJ) $(CPPFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJ)