CXX = /usr/bin/g++
CXXFLAGS = -c -Wall -Wno-reorder -std=c++11
INCLUDES = -I/usr/X11R6/include
LIBS =  -L/usr/X11R6/lib -lX11 -lcairo
OBJ_DIR = ./objects
SOURCES = button.cpp circuit.cpp component.cpp controller.cpp graphical.cpp main.cpp node.cpp
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)
EXECUTABLE = circuit-solver

all: dirs $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LIBS) $(OBJECTS) -o $(EXECUTABLE)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@

dirs:
	mkdir -p $(OBJ_DIR)

clean:
	rm -r $(OBJ_DIR)
	rm $(EXECUTABLE)

.PHONY: dirs all clean
