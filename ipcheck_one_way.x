/*
 * ipcheck_one_way.x: no-reply remote multiple directory listing protocol
 */
const           MAXNAMELEN = 255;
typedef string  nametype < MAXNAMELEN >;        /* a directory entry */

/*
 * The directory program definition, two procedures, one expects nothing, the
 * other retrieves any results.
 */
program         DIRPROG {
  version DIRVERS {
    void        checkIP(nametype) = 1;
    int         getResult()       = 2;
  } =   1;
} =             0x20000002;
