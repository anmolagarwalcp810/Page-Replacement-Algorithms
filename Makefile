all : final

final: bin_files
	./bin/main $(ARGS)
	./bin/make_graph
	
bin_files : object_files
	gcc -o ./bin/make_graph ./obj/make_graph.o
	gcc -o ./bin/main ./obj/main.o

object_files :
	gcc -g -O -c ./src/make_graph.c
	gcc -g -O -c ./src/main.c
	mv *.o ./obj/

clean :
	rm -rf *.o
	rm -rf obj/*.o
	rm -rf bin/*