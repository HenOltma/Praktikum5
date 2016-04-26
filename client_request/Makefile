EXE = dir
MAIN = ipcheck_one_way_dir_client
RPCGEN = rpcgen

EXE: $(EXE)

ipcheck_one_way_xdr.c ipcheck_one_way_clnt.c ipcheck_one_way.h: ../ipcheck_one_way.x
	cp ../ipcheck_one_way.x .
	$(RPCGEN) ipcheck_one_way.x
	$(RM) ipcheck_one_way_svc.c
	sed 's/static struct timeval TIMEOUT = { 25, 0 }/extern struct timeval TIMEOUT/' \
	< ipcheck_one_way_clnt.c > /tmp/$$; mv /tmp/$$ ipcheck_one_way_clnt.c

ipcheck_one_way_xdr.o: ipcheck_one_way_xdr.c ipcheck_one_way.h
	gcc -c ipcheck_one_way_xdr.c

ipcheck_one_way_clnt.o: ipcheck_one_way_clnt.c ipcheck_one_way.h
	gcc -c ipcheck_one_way_clnt.c

$(EXE): ipcheck_one_way_xdr.o ipcheck_one_way_clnt.o $(MAIN).c
	gcc -o $(EXE) ipcheck_one_way_xdr.o ipcheck_one_way_clnt.o $(MAIN).c

clean:
	$(RM) $(EXE) ipcheck_one_way_xdr.c ipcheck_one_way_clnt.c \
	ipcheck_one_way.h ipcheck_one_way.x *.o *.*% 
