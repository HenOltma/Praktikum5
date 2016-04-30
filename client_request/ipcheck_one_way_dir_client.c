/**************************************************************  
 * ipcheck_one_way_dir_client.c	
 * Client ruft RLS Server asynchron auf. 
 * Heinz-Josef Eikerling, HS OS
 * 14.06.2012: Fertigstellung
 * To Do:
 * - %
 **************************************************************/

#include <stdio.h>
#include <rpc/rpc.h>
#include <errno.h>
#include "ipcheck_one_way.h"

extern int errno;
struct timeval TIMEOUT = {0, 0}; /* used by one_way_clnt.c with clnt_call() timeouts */

/*
 * ruft die Funktion checkIP() auf dem Server auf.
 * @param {nametype} Die IP-Adresse
 */
void checkIP(char *host, nametype *ip){
    	CLIENT  *clnt;
	int     *result_1;
	nametype  ip_adress = *ip;

        printf("%s\n", ip_adress);
	printf("Erstelle Client...\n");

	clnt = clnt_create (host, CHECK_IP, CHECK_IP_1, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	printf("Client erfolgreich erstellt!\n");
	printf("Stelle Anfrage an Server...\n");

	result_1 = checkip_1(&ip_adress, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	printf("Anfrage erfolgreich!");
}
/*
 * main function - kontrolliert ob 2 Argumente übergeben wurden und ruft checkIP() auf.
 */
int main(int argc, char** argv)
{
    char *host;
    int result;
    nametype ip_adress = argv[2];

    printf("Anzahl der Argumente: %d\n", argc);

    if (argc < 3) {
            printf ("usage: %s: <client ip> <zu überprüfende adresse>\n", argv[0]);
            exit (1);
    }
    host = argv[1];
    printf("host: %s\n", host);
    printf("ip: %s\n", ip_adress);
    checkIP (host, &ip_adress);
    exit (0);
}
