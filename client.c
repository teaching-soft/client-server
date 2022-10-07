/***************************************************************************
	file				 : client.c
    begin                : 2019
    copyright            : (C) 2019 by Giancarlo Martini and friends
    email                : gm@giancarlomartini.it

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

*/#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#define SERVER_IP	"127.0.0.1"
#define SERVER_PORT	"5555"

/* Funzione per uscire dal programma segnalando l'errore */
void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sock_fd, port_num, n;
	/* sockaddr_in: Contiene le informazioni di un host sulla rete */
	struct sockaddr_in server_addr;

	/* Copia l'indirizzo del server */
	char server_ip[20];
	strcpy(server_ip, SERVER_IP);

	/* Prende il numero di porta passato sulla riga di comando,
	   atoi converte la stringa argv[2] in valore numerico */
	port_num = atoi(SERVER_PORT);

	/* Crea il socket */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	/* Controllo sull'operazione */
	if (sock_fd < 0)
		error("ERROR opening socket");

	/* Predispone la struttura che deve contenere le informazioni del server */
	bzero((char *)&server_addr, sizeof(server_addr));

	/*  Riempe la struttura con le informazioni della connessione
	   Tipo di socket family = AF_INET,
	   Porta con relativa conversione sin_port = htons(port_num);
	   La funzione htonl() converte il valore da host order a network order
	   La funzione inet_pton converte un ip in formato stringa nella forma
	   binaria */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_num);

	if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
		error("IP non valido");

	/* Cerca di connettersi al server */
	if (connect(sock_fd, (const struct sockaddr *)&server_addr,
		    sizeof(server_addr)) < 0)
		error("ERROR connecting");

	/* Si è connesso, chiede all'operatore il messaggio da inviare */
	printf("Inserisci il messaggio: ");

	/* Legge la tastiera */
	char buffer[256];
	fgets(buffer, 255, stdin);

	/* Lo invia in rete */
	n = write(sock_fd, buffer, strlen(buffer));
	if (n < 0)
		error("ERROR writing to socket");

	/* Legge la risposta */
	n = read(sock_fd, buffer, 255);
	if (n < 0)
		error("ERROR reading from socket");
	buffer[n] = '\0';

	/* Stampa la risposta */
	printf("%s\n", buffer);

	/* Chiude il socket */
	close (sock_fd);
	return 0;
}
