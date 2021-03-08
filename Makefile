CFLAGS=-g -std=c++17 -Wall -pedantic -fpermissive

main: main.cpp
	$(CXX) $(CFLAGS) $? $(LDFLAGS) -o next

clean:
	rm next
