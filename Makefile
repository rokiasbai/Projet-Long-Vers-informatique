build : serveur/client.c serveur/server.c worm/worm.c
	gcc serveur/client.c -g -o serveur/client.out
	gcc -fno-stack-protector -z execstack serveur/server.c -g -o serveur/server.out 
	gcc worm/worm.c -o worm/worm.out
clean :
	rm -f *.out
