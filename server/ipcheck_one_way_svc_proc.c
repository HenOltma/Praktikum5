/**************************************************************
 * ipcheck_one_way_svc_proc.c
 * Implementierung der Funktionen zum Auslesen von Directories.
 * Die erste Funktion (dirname_1_svc) kann asynchnron aufgerufen werden.
 * Die zweite Funktion (readdir_1_svc) kann nur synchron aufgerufen werden.
 * Heinz-Josef Eikerling, HS OS
 * 16.06.2012: Fertigstellung
 * 01.05.2014: Korrektur beim Zurücksetzen der Ergebnisliste.
 * To Do:
 * - Ggf. Struktur mit Listen fuer jedes Verzeichnis erzeugen.
 **************************************************************/

#include <rpc/rpc.h>
#include <sys/dir.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ipcheck_one_way.h"

extern int errno;

/* Struktur zur Speicherung von Directory-Listings pro Client! */
typedef struct resnode* readdir_reslist;
struct resnode {
	char* requesting_clnt_addr;
    readdir_res* check_res;
	readdir_reslist pNext;
};
typedef struct resnode resnode;

/* Ergebnisliste:  muss statisch sein! */
static readdir_reslist reslist = NULL;

/* Default (leere) Ergebnisliste:  muss statisch sein! */
static readdir_res DefaultResult;

/* IP-Adresse des letzten Clients, für den das Ergebnis geloescht werden soll */
static char* reset_client_addr = NULL;

/*
 * Zuruecksetzen der Ergebnisliste fuer
 * einen durch req_clnt_addr gegebenen Client.
 */
void reset_result_list(char* req_clnt_addr) {
    if (req_clnt_addr == NULL)
        return;

    resnode* cursor = reslist;
    resnode* pcursor = NULL;
    while (cursor != NULL) {
        if (strcmp(cursor->requesting_clnt_addr, req_clnt_addr) == 0) {
            printf("Removing entry for %s\n", req_clnt_addr);
            readdir_res* res = cursor->check_res;
            xdr_free((xdrproc_t) xdr_readdir_res, (char *) &res);
            if (pcursor == NULL) /* Löschen am Listenanfang */
                reslist = cursor->pNext;
            else /* Löschen in der Mitte der Liste */
                pcursor->pNext = cursor->pNext;
            free(cursor->requesting_clnt_addr);
            free(res);
            /*  Loeschung erfolgt, Merker zuruecksetzen */
            reset_client_addr = NULL;
            return;
        }
        pcursor = cursor;
        cursor = cursor->pNext;
    }
}

/*
 * Suchen der Ergebnisliste fuer
 * einen durch req_clnt_addr gegebenen Client.
 */
readdir_res* find_result_for_client (char* req_clnt_addr) {
    resnode* cursor =  reslist;
    while (cursor != NULL) {
        if (strcmp(cursor->requesting_clnt_addr, req_clnt_addr) == 0) {
            printf("Matching entry found for %s\n", req_clnt_addr);
            return cursor->check_res;
        }
        cursor = cursor->pNext;
    }
    printf("No matching entry found for %s\n", req_clnt_addr);
    return NULL;
}

/*
 * Neue Ergebnisliste res fuer
 * einen durch req_clnt_addr gegebenen Client eintragen.
 */
void enter_result_for_client(readdir_res* res, char* req_clnt_addr) {
    printf("Entering result list for %s\n", req_clnt_addr);
    resnode* new_res_node = (resnode *) malloc(sizeof (resnode));
    new_res_node->requesting_clnt_addr = strdup(req_clnt_addr);
    new_res_node->check_res = res;
    /* Einfuegen am Anfang der Liste */
    new_res_node->pNext = reslist;
    reslist = new_res_node;
    return;
}

void* dirname_1_svc(nametype *dirname, struct svc_req *request) {
    namelist nl;
    namelist *nlp;
    DIR *dirp;
    struct direct *d;

    /*
     * Ggf. Ergebnisse von letztem Client zurueck setzen.
     */
    reset_result_list (reset_client_addr);

    /* Adresse des anfragenden Clients bestimmen */
    char* req_addr = inet_ntoa(request->rq_xprt->xp_raddr.sin_addr);
    readdir_res* res = find_result_for_client (req_addr);

    if (res == NULL) {
        /* neue Liste anlegen */
        res = (readdir_res*) malloc (sizeof(readdir_res));
        /* Das ist wichtig hier! Bei einer statischen Variable nicht!*/
        res->readdir_res_u.list = NULL;
        enter_result_for_client (res, req_addr);
    }

    /* Verzeichnis zum Auslesen oeffnen.  */
    dirp = opendir(*dirname);
    if (dirp == NULL) {
        res->remoteErrno = errno;
        return (NULL);
    }

    /* Eintrag erzeugen */
    char cbuffer [256];
    sprintf(cbuffer, "************* %s *************", *dirname);
    printf("%s\n", cbuffer);

    /* An das Ende Result-Liste gehen. */
    nlp = &(res->readdir_res_u.list);
    while ((nl = *nlp) != NULL)
        nlp = &(nl->pNext);

    nl = (namenode *) malloc(sizeof (namenode));
    *nlp = nl;
    nl->name = strdup(cbuffer);
    nlp = &(nl->pNext);

    /* Verzeichnis auslesen und Inhalt in Liste speichern. */
    while (d = readdir(dirp)) {
        nl = *nlp = (namenode *) malloc(sizeof (namenode));
        nl->name = strdup(d->d_name);
        nlp = &nl->pNext;
    }
    *nlp = NULL;

    /* Verzeichnis schliessen. Es wird kein Ergebnis zurueck gegeben. */
    res->remoteErrno = 0;
    closedir(dirp);
    return (NULL);
}

readdir_res* readdir_1_svc(void * dummy, struct svc_req *request) {
    /* Adresse des anfragenden Clients bestimmen */
    char* req_addr = inet_ntoa(request->rq_xprt->xp_raddr.sin_addr);

    /* Ggf. letzte Ergebnisse von letztem Client zurueck setzen. */
    reset_result_list(reset_client_addr);

    /* Ergebnis suchen */
    readdir_res* res = find_result_for_client(req_addr);

    if (res == NULL) {
        res = &DefaultResult;
    }
    else {
        /* Ergebnisse gelesen -> fuer Loeschung vorsehen */
        reset_client_addr = req_addr;
    }
    return (res);
}
