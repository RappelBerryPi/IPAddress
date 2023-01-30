clean:
	del *.exe
	del *.obj
	del *.lib
	del *.exp
	del *.dll

dll: clean
	cl.exe /LD .\NetworkInterface.cpp .\IP.cpp

exe: dll
	cl.exe .\program.cpp /link .\NetworkInterface.lib

all: exe

default: all