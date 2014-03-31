CXXFLAGS=-Wall -Wextra -std=c++0x -lbluetooth

sources=main.cpp
classes=BT_Device.cpp BT_Controler.cpp 
headers=$(classes:.cpp=.h)

all: $(sources:.cpp=)

main: main.o BT_Device.o BT_Controler.o

%: %.o
	$(LINK.cpp) -o $@ $^

clean:
	rm -fv *~
	rm -fv *.o

depend:
	makedepend -Y $(sources) $(classes)
# DO NOT DELETE

main.o: BT_Controler.h rssi.h BT_Device.h
BT_Device.o: BT_Device.h
BT_Controler.o: BT_Controler.h rssi.h BT_Device.h
