CXX = g++
FLAGS = -Wall -lncursesw
EXE = snake

all: snake.cpp
	$(CXX) $(FLAGS) snake.cpp -o $(EXE)
	./$(EXE)

run: $(EXE)
	./$(EXE)

clean:
	rm $(EXE)
