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

int main(int argc, char** argv)
{
    CLIENT *cl;
    char *server;
    char *dir;
    readdir_res *result;
    namelist nl;
    int i;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s host directory(s)\n", argv[0]);
        exit(1);
    }
    server = argv[1];

    /* 
     * Erzeugung eines Client Handles. 
     * Fuer asynchrone One-way-Aufrufe wird hier TCP eingestellt, 
     * damit der Aufruf in jedem Fall den Server erreicht.
     */
    if ((cl = clnt_create(server, DIRPROG, DIRVERS, "tcp")) == NULL) {
        clnt_pcreateerror(server);
        exit(1);
    }
    /*
     * Fuer alle Argumente der Kommandozeile wird die Server-Funktion 
     * aufgerufen. Der Timeout wird auf 0 gesetzt, auf die Antwort 
     * muss (und sollte) nicht gewartet werden.
     */
    TIMEOUT.tv_sec = TIMEOUT.tv_usec = 0;
    if (clnt_control(cl, CLSET_TIMEOUT, (char*) &TIMEOUT) == FALSE) {
        fprintf (stderr, "can't zero timeout\n");
        exit(1);
    }
    for (i = 2; i < argc; i++) {
        fprintf (stdout, "Calling rls for '%s'\n", argv[i]);        
        dirname_1(&(argv[i]), cl);
        clnt_perror(cl, server); /* ignore the time-out errors */
    }
    exit(0);
}
