/**************************************************************
 * rls_one_way_svc_proc.c
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include "ipcheck_one_way.h"

int validateIP(nametype *argp, struct svc_req *rqstp);
int validateSubnetmask(int ipv4[4], int maske[4],char* prefix);
int validateIPv4Address(char* address, int ipv4[4]);
int parseIPv4(char* address, unsigned int* clientIP, uint32_t* subnetmask);


/* Struktur zur Speicherung von Directory-Listings pro Client! */
typedef struct resnode* readdir_reslist;
struct resnode {
	char* requesting_clnt_addr;
    int checkIP_result;
	readdir_reslist pNext;
};
typedef struct resnode resnode;

/* Ergebnisliste:  muss statisch sein! */
static readdir_reslist global_results = NULL;

/* IP-Adresse des letzten Clients, für den das Ergebnis geloescht werden soll */
static char* reset_client_addr = NULL;

/*
 * Zuruecksetzen der Ergebnisliste fuer
 * einen durch req_clnt_addr gegebenen Client.
 */
void reset_result_list(char* req_clnt_addr) {
    if (req_clnt_addr == NULL)
        return;

    resnode* cursor = global_results;
    resnode* pcursor = NULL;
    while (cursor != NULL) {
        if (strcmp(cursor->requesting_clnt_addr, req_clnt_addr) == 0) {
            printf("Removing entry for %s\n", req_clnt_addr);
            if (pcursor == NULL) /* Löschen am Listenanfang */
                global_results = cursor->pNext;
            else /* Löschen in der Mitte der Liste */
                pcursor->pNext = cursor->pNext;
            free(cursor->requesting_clnt_addr);
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
int* find_result_for_client (char* req_clnt_addr) {
    resnode* cursor =  global_results;
    while (cursor != NULL) {
        if (strcmp(cursor->requesting_clnt_addr, req_clnt_addr) == 0) {
            printf("Matching entry found for %s\n", req_clnt_addr);
            return &(cursor->checkIP_result);
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
void enter_result_for_client(int res, char* req_clnt_addr) {
    printf("Entering result list for %s\n", req_clnt_addr);

    resnode* new_res_node = (resnode *) malloc(sizeof (resnode));
    new_res_node->requesting_clnt_addr = strdup(req_clnt_addr);
    new_res_node->checkIP_result = res;
    /* Einfuegen am Anfang der Liste */
    new_res_node->pNext = global_results;
    global_results = new_res_node;
    return;
}

/* Schnittstelle um IPs zu überprüfen */
void* checkip_1_svc(nametype* ipadress, struct svc_req *request) {
    /*
     * Ggf. Ergebnisse von letztem Client zurueck setzen.
     */
    reset_result_list (reset_client_addr);

    /* Adresse des anfragenden Clients bestimmen */
    char* req_addr = inet_ntoa(request->rq_xprt->xp_raddr.sin_addr);

	/* IP-Adresse validieren */
    int res = validateIP(ipadress, request);

	/* Ergebnis in Liste eintragen */
    enter_result_for_client (res, req_addr);

	return (NULL);
}

/* Schnittstelle um Ergebnisse abzurufen */
int* getresult_1_svc(void * dummy, struct svc_req *request) {
    
    printf("Hole Ergebnis...");
    
    /* Adresse des anfragenden Clients bestimmen */
    char* req_addr = inet_ntoa(request->rq_xprt->xp_raddr.sin_addr);

    /* Ggf. letzte Ergebnisse von letztem Client zurueck setzen. */
    reset_result_list(reset_client_addr);

    /* Ergebnis suchen */
    int* res = find_result_for_client(req_addr);

    /* Ergebnisse gelesen -> fuer Loeschung vorsehen */
    reset_client_addr = req_addr;

    return res;
}

/*****************************************************************
				Aus letztem Praktikum übernommen
******************************************************************/

int validateIP(nametype *argp, struct svc_req *rqstp) {
	static int  result;
        uint32_t clientIP;
        uint32_t serverIP;
        uint32_t subnetmask;
        result = 0;

        printf("\n\nGot Client request:\t'Is %s in the server subnet?'\n\n", *argp);
        result = parseIPv4(*argp, &clientIP, &subnetmask);
        serverIP = rqstp->rq_xprt->xp_raddr.sin_addr.s_addr;
		printf("server IP: %s\n",inet_ntoa(rqstp->rq_xprt->xp_raddr.sin_addr));
        struct in_addr netIP;
        netIP.s_addr = clientIP & subnetmask;
        printf("net IP: %s\n",inet_ntoa(netIP));
        if(result == 0){
            if(clientIP == serverIP){
                printf("same server/client address");
            }
            else if(netIP.s_addr == clientIP){
                printf("netaddress\n");
                result = 6;
            }
            else if(clientIP == (clientIP | (~ subnetmask))){
                printf("broadcast address\n");
                result = 5;
            }
            else if(!((clientIP & subnetmask) == (serverIP & subnetmask))){
                result = 4;
                printf("not in the same subnet.\n");
            }
        }
        return result;
}
// Validates Subnetmask. returns 0 on success. 1 on failure.
int validateSubnetmask(int ipv4[4], int maske[4],char* prefix){
        if(prefix == NULL){
            printf("no subnetmask! Please input the address 'x.x.x.x/subnetmask', x in range '0-255' and subnetmask in range '0-30'.\n");
            return 1;
	}
        int prefix_int = atoi(prefix);
    	if(prefix_int >30 || prefix_int <0){
            printf("invalid subnetmask! Subnetmaskprefix has to be in the range of 0-30.\n");
            return 1;
	}
        int tmp1 = prefix_int/8;
        int i;
        for(i = 0; i < tmp1; i++){
            maske[i] = 255;
        }
        for(i = 0; i != prefix_int%8; i++){
            // maske[tmp1] += pow(2, 7-i); TODO: hier wieder einkommentieren und irgendwie lösen
        }
        printf("subnetmask = %d.%d.%d.%d is valid.\n",maske[0],maske[1],maske[2],maske[3]);
        return 0;
}
// Validates a IPv4 address. returns 0 on success. 2 on failure.
int validateIPv4Address(char* address, int ipv4[4]){
        char *tmpStr;
        tmpStr = strtok(address,".");
        int i;
        for(i = 0; i < 4; i++){
            if(tmpStr == NULL){
                printf("invalid address! Address have to be of the following syntax: 'x.x.x.x/netmask'. x has to be between 0 and 255. netmask has to be between 0 and 30.\n");
                return 2;
            }
            ipv4[i] = atoi(tmpStr);
            if(ipv4[i] < 0 || ipv4[i] > 255){
                printf("invalid address! Only numbers between 0 and 255 are allowed.\n");
                return 2;
            }
            tmpStr = strtok(NULL,".");
        }

        printf("ip = %d.%d.%d.%d is valid!\n",ipv4[0],ipv4[1],ipv4[2],ipv4[3]);
        return 0;
}
// Parses the string 'IPv4 address/subnet suffix' into unsigned 32 bit integer clientIP & subnetmask. Returns 0 on success or a error code on failure.
int parseIPv4(char* address, unsigned int* clientIP, uint32_t* subnetmask){
        char* ip;
        char* prefix;
        int ipv4[4] = {0,0,0,0};
        int maske[4] = {0,0,0,0};
        int returnValue;
        //cuts the IPv4/Prefix into Parts.
        ip = strtok(address,"/");
	prefix = strtok(NULL,"");

        returnValue = validateIPv4Address(address, ipv4);
        returnValue += validateSubnetmask(ipv4, maske, prefix);

        // wandelt ip-address int array zu 32-bit int um.
        *clientIP = ((unsigned char)ipv4[3] << 24) | ((unsigned char)ipv4[2] << 16) | ((unsigned char)ipv4[1] << 8) | (unsigned char)ipv4[0];
        *subnetmask = ((unsigned char)maske[3] << 24) | ((unsigned char)maske[2] << 16) | ((unsigned char)maske[1] << 8) | (unsigned char)maske[0];

        return returnValue;
}
