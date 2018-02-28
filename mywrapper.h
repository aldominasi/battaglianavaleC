#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <netdb.h>

int CreaSocket(int famiglia, int tipo, int protocollo);
void Connessione(int sockfd, struct sockaddr_in servaddr);
void AssegnaIndirizzo(int sockfd, struct sockaddr_in servaddr);
void Ascolto(int sockfd, int lengthQueue);
void ImpostaIndirizzoClient(int famiglia, char *addr, int porta, struct sockaddr_in *servaddr);
void ImpostaIndirizzoAnyServer(int famiglia, int porta, struct sockaddr_in *servaddr);
int AccettazioneNuovaRichiesta(int sockfd);
int AccettazioneNuovaRichiestaConcorrente(int sockfd, struct sockaddr_in *client, socklen_t *len);
ssize_t FullWrite(int fd, const void *buf, size_t count);
ssize_t FullRead(int fd, void *buf, size_t count);
