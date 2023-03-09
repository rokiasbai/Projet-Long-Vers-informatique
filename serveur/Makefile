build : client_propre.c server.c
	gcc client_propre.c -g -o client_propre.out
	gcc -fno-stack-protector -z execstack server.c -g -o server.out 

clean :
	rm -f *.out
