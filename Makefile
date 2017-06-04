EXECUTABLE= CA
SOURCES = main.cpp cell.cpp state.cpp utils.cpp automaton.cpp slump.cpp stats.cpp 
HEADERS = main.h cell.h state.h utils.h automaton.h slump.h stats.h 
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