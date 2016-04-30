EXE = ipcheck_one_way_session_server
MAIN = ipcheck_one_way_svc
RPCGEN = rpcgen

EXE: $(EXE)

ipcheck_one_way_svc.c ipcheck_one_way_xdr.c ipcheck_one_way.h: ../ipcheck_one_way.x
	cp ../ipcheck_one_way.x .
	$(RPCGEN) ipcheck_one_way.x
	$(RM) $(MAIN).c
	$(RPCGEN) -s tcp -o $(MAIN).c ipcheck_one_way.x
	$(RM) ipcheck_one_way_clnt.c

$(EXE): $(MAIN).c ipcheck_one_way_svc_proc.c ipcheck_one_way_xdr.c ipcheck_one_way.h
	gcc -o $(EXE) ipcheck_one_way_svc_proc.c ipcheck_one_way_xdr.c $(MAIN).c

clean:
	$(RM) $(EXE) $(MAIN).c ipcheck_one_way_xdr.c \
	ipcheck_one_way.h ipcheck_one_way.x *.o *.*%
