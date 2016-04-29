/*
 * ipcheck_one_way.x: no-reply remote multiple ip-check protocol
 */

/* max. länge von IPv.4 in "dotted decimal notation" */
const MAXLEN = 19;


typedef string ip_str<MAXLEN>;			/* string für ip adresse */
typedef struct  resultnode *resultlist;      	/* a link in the listing */


/*
 * a node in the directory
 */
struct resultnode {
  ip_str        name;         	/* string für ip adresse */
  int		result;		/* Rückgabe Code 0 = success */	
  resultlist	pNext;		/* next entry */
};

/*
 * the result of a READDIR operation
 */
union checkIP_res switch (int remoteErrno) {
  case 0:
  resultlist list;                /* no error: return directory listing */
default:
  void;                         /* error occurred: nothing else to return */
};


/*
 * The IP-Check program definition, two procedures, one expects nothing, the
 * other retrieves any results.
 */
program         CHECK_IP {
  version CHECK_IP_1 {
    void        CHECKIP(ip_str) = 1;
    checkIP_res	GETRESULT()       = 2;
  } =   1;
} =             0x20000002;
