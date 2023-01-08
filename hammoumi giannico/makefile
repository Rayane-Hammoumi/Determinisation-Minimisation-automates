SOURCES = main.c execution_mots.c determinisation.c minimisation.c fct_auxiliaires.c
OUTPUT = Detmin
OBJ = $(SOURCES:.c=.o)

all: $(OUTPUT)

clean: 
	rm $(OUTPUT) *.o ./*.o

$(OUTPUT): $(OBJ)
	@gcc $^ -o $(OUTPUT)
	rm *.o