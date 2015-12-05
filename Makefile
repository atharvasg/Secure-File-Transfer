all:
		gcc -o SERVER server.c
		gcc -o CLIENT client.c

clean:
		rm -f CLIENT SERVER
