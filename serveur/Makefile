build : client.c server.c
	gcc -Wall client.c -g -o client.out
	gcc -fno-stack-protector -z execstack -Wall server.c -g -o server.out 

clean :
	rm -f *.out
