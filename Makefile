EXECUTABLE= CA
SOURCES = src/main.cpp src/cell.cpp src/state.cpp src/utils.cpp src/automaton.cpp src/slump.cpp src/stats.cpp 
HEADERS = src/main.h src/cell.h src/state.h src/utils.h src/automaton.h src/slump.h src/stats.h 
CFLAGS= -Wall -Wextra -std=c++11 -g
NAME=04_xbenom01_xstude22

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES) $(HEADERS)
	g++ $(SOURCES) lib/EasyBMP.cpp lib/pugixml.cpp -o $(EXECUTABLE) $(CFLAGS)

run: all
	./$(EXECUTABLE) -i config.xml

memtest: all
	valgrind ./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

pack:
	cp doc/$(NAME).pdf .
	zip $(NAME).zip *.cpp *.h $(NAME).pdf *.xml lib/* Makefile