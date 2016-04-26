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
    char *dir;
    readdir_res *result;
    namelist nl;
    int i;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s host\n", argv[0]);
        exit(1);
    }
    server = argv[1];

    /* 
     * Erzeugung eines Client Handles.
     */
    if ((cl = clnt_create(server, DIRPROG, DIRVERS, "tcp")) == NULL) {
        clnt_pcreateerror(server);
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
    if ((result = readdir_1(&(argv[i]), cl)) == NULL) {
        clnt_perror(cl, server);
        exit(1);
    }
    
    /* Fehler auswerten. */
    if (result->remoteErrno != 0) {
        errno = result->remoteErrno;
        perror(dir);
        exit(1);
    }
    
    /* Ergebnis ausgeben. */
    for (nl = result->readdir_res_u.list; nl != NULL; nl = nl->pNext) {
        printf("%s\n", nl->name);
    }
    exit(0);
}
