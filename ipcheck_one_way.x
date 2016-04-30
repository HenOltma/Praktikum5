/*
 * ipcheck_one_way.x: no-reply remote multiple ip-check protocol
 */
const           MAXLEN = 19;
const           MAXNAMELEN = 255;
typedef string  nametype < MAXNAMELEN >;        /* a directory entry */

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
