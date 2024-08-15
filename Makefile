CXX = g++
FLAGS = -Wall -lncurses
EXE = snake

all: snake.cpp
	$(CXX) $(FLAGS) snake.cpp -o $(EXE)

run: $(EXE)
	./$(EXE)

clean:
	rm $(EXE)
