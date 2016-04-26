# Praktikum5

### Funktionen

```c
/**
 * Checkt die übergebene IP-Adresse und speichert das Ergebnis in der
 * Liste auf dem Server, sodass es später abgerufen werden kann.
 * @param {nametype} Die IP-Adresse
 */
void checkIP(nametype);

/**
 * Ruft das Ergebnis des IP-Checks vom Server ab. Dabei wird das Ergebnisse
 * aus der Liste auf dem Server gelöscht.
 * @return {int} Der Rückgabecode
 */
int getResult();
```

### Rückgabecodes:
```
0 - success
1 - invalid subnetmask
2 - invalid ip-address
3 - invalid subnetmask && invalid ip-address
4 - nicht im subnetz
5 - Adresse ist Broadcastadresse
6 - Adresse ist Routeradresse
```
