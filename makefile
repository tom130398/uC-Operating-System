OBJ = string.o stringhw.o
APPNAME = string.a

$(APPNAME) : $(OBJ)
	gcc -o $(APPNAME) $(OBJ) -lm

%.o : %.c
	gcc -c -o $@ $< -std=c99

clean : 
	rm $(OBJ) $(APPNAME)

