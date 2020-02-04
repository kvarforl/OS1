CC= gcc -g -Wall
exe1 = kvarforl.buildrooms
exe2 = kvarforl.adventure
game:
	$(CC) kvarforl.adventure.c -lpthread -o $(exe2)
rooms:
	$(CC) kvarforl.buildrooms.c -o $(exe1)
clean:
	rm -rf *.o *.out $(exe1) $(exe2) 
