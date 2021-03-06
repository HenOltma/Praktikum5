/**************************************************************  
 * ipcheck_one_way_result_client.c	
 * Client ruft vom RLS Server synchron die Ergebnisse ab. 
 * Heinz-Josef Eikerling, HS OS
 * 15.06.2012: Fertigstellung
 * 01.05.2014: Abfrage der Eingabeparameter korrigiert
 * To Do:
 * - %
 **************************************************************/

#include <stdio.h>
#include <rpc/rpc.h>
#include <errno.h>
#include "ipcheck_one_way.h"

extern int errno;
struct timeval TIMEOUT = {0, 0}; /* used by one_way_clnt.c with clnt_call() timeouts */

int main(int argc, char** argv)
{
    CLIENT *cl;
    char *server;
    int *result;
    int i;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s host\n", argv[0]);
        exit(1);
    }
    server = argv[1];

    /* 
     * Erzeugung eines Client Handles.
     */
    printf("erzeuge client handle...\n");
    if ((cl = clnt_create(server, CHECK_IP, CHECK_IP_1, "tcp")) == NULL) {
        clnt_pcreateerror(server);
        exit(1);
    }
    
    /*
     * Abruf der Ergebnisse vom Server. 
     * Der Timeout wird auf 25s (Default) gesetzt.
     */
    printf("Setze timeout...\n");
    TIMEOUT.tv_sec = 25;
    if (clnt_control(cl, CLSET_TIMEOUT, (char*) &TIMEOUT) == FALSE) {
        fprintf (stderr, "can't zero timeout\n");
        exit(1);
    }
    
    /*
     * Abruf der Ergebnisse vom Server. 
     * Der Timeout wird auf 25s (Default) gesetzt.
    */
    TIMEOUT.tv_sec = 25;
    if (clnt_control(cl, CLSET_TIMEOUT, (char*) &TIMEOUT) == FALSE) {
       fprintf (stderr, "can't zero timeout\n");
       exit(1);
    }
        
    printf("Schicke Anfrage ab...\n");
    if ((result = getresult_1(&(argv[i]), cl)) == NULL) {
        clnt_perror(cl, server);
        exit(1);
    }
    
    /* Ergebnis ausgeben. */
    printf("Ergebnis:\n");
//     printf("| %s\t| ", nl->name);
    switch (*result) {
            case 0:
                    printf("IP-Adresse ist gültig und befindet sich im Subnetz des Hosts");
                    break;

            case 1:
                    printf("Die Subnetzmaske ist ungülitg");
                    break;

            case 2:
                    printf("IP-Adresse ist ungültig");
                    break;

            case 3:
                    printf("IP-Adresse und Subnetzmaske sind ungültig");
                    break;

            case 4:
                    printf("IP-Adresse befindet sich nicht im Subnetz des Hosts");
                    break;

            case 5:
                    printf("IP-Adresse ist die Broadcastadresse des Subnetzes");
                    break;

            case 6:
                    printf("IP-Adresse ist die Routeradresse des Subnetzes");
                    break;

            case -1:
                    printf("Kein Eintrag zur IP-Adresse gefunden");
                    break;

            default:
                    printf("Ungültige Antwort vom Server!");
    
    printf("\t|\n");
    }
    exit(0);
}
