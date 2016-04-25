EXE = dirresults
MAIN = rls_one_way_result_client
RPCGEN = rpcgen

EXE: $(EXE)
 
rls_one_way_xdr.c rls_one_way_clnt.c rls_one_way.h: ../rls_one_way.x
	cp ../rls_one_way.x .
	$(RPCGEN) rls_one_way.x
	$(RM) rls_one_way_svc.c
	sed 's/static struct timeval TIMEOUT = { 25, 0 }/extern struct timeval TIMEOUT/' \
	< rls_one_way_clnt.c > /tmp/$$; mv /tmp/$$ rls_one_way_clnt.c

rls_one_way_xdr.o: rls_one_way_xdr.c rls_one_way.h
	gcc -c rls_one_way_xdr.c

rls_one_way_clnt.o: rls_one_way_clnt.c rls_one_way.h
	gcc -c rls_one_way_clnt.c

$(EXE): rls_one_way_xdr.o rls_one_way_clnt.o $(MAIN).c
	gcc -o $(EXE) rls_one_way_xdr.o rls_one_way_clnt.o $(MAIN).c

clean:
	$(RM) $(EXE) rls_one_way_xdr.c rls_one_way_clnt.c \
	rls_one_way.h rls_one_way.x *.o *.*% 
