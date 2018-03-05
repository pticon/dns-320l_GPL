INCLUDES = -I../include -I.
CFLAGS = -DMONOLITH $(INCLUDES) $(CFLAG)
LIBCRYPTO=-L.. -lcrypto
LIBSSL=-L.. -lssl

PROGRAM= openssl
PROGS= $(PROGRAM).c

A_OBJ=apps.o
A_SRC=apps.c
S_OBJ=  s_cb.o s_socket.o
S_SRC=  s_cb.c s_socket.c
RAND_OBJ=app_rand.o
RAND_SRC=app_rand.c

E_OBJ=  verify.o asn1pars.o req.o dgst.o dh.o dhparam.o enc.o \
		passwd.o gendh.o errstr.o \
		ca.o pkcs7.o crl2p7.o crl.o \
		rsa.o rsautl.o dsa.o dsaparam.o \
		x509.o genrsa.o gendsa.o s_server.o s_client.o speed.o \
		s_time.o $(A_OBJ) $(S_OBJ) $(RAND_OBJ) version.o sess_id.o \
		ciphers.o nseq.o pkcs12.o pkcs8.o spkac.o smime.o \
		rand.o engine.o ocsp.o

E_SRC=  verify.c asn1pars.c req.c dgst.c dh.c enc.c \
		passwd.c gendh.c errstr.c ca.c \
		pkcs7.c crl2p7.c crl.c \
		rsa.c rsautl.c dsa.c dsaparam.c \
		x509.c genrsa.c gendsa.c s_server.c s_client.c speed.c \
		s_time.c $(A_SRC) $(S_SRC) $(RAND_SRC) version.c sess_id.c \
 		ciphers.c nseq.c pkcs12.c pkcs8.c spkac.c smime.c \
		rand.c engine.c ocsp.c		 

SRC=$(E_SRC)

all:    exe

exe:    $(PROGRAM)

clean:
	rm -f *.o *.obj lib tags core .pure .nfs* *.old *.bak fluff $(PROGRAM)
	rm -f req

$(PROGRAM): progs.h $(E_OBJ) $(PROGRAM).o
	rm -f $(PROGRAM)
	$(CC) -s -o $(PROGRAM) $(CFLAGS) $(PROGRAM).o $(E_OBJ) $(PEX_LIBS) $(LIBSSL) $(LIBKRB5) $(LIBCRYPTO) $(EX_LIBS)

