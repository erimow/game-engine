#OBJS specifies which files to compile as part of the project
OBJS = main.c #Unused

SOURCES := $(wildcard ./*.c) $(wildcard Engine/*.c)

#CC specifies which compiler we're using
CC = gcc

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w -Wall -Wextra -g -ggdb3 -O0

#INCLUDES
INCLUDES = -I./Sounds -I./Art -I./Engine -I./Fonts

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = play

#This is the target that compiles our executable
all : $(SOURCES)
	$(CC) $(SOURCES) $(COMPILER_FLAGS) $(INCLUDES) $(LINKER_FLAGS) -o $(OBJ_NAME)
