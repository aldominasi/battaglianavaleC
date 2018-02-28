#include "mywrapper.h"

/*
 * Crea il socket, utilizzata sia lato client che lato server
 */
int CreaSocket(int famiglia, int tipo, int protocollo)
{
	int n;
	if ((n = socket(famiglia,tipo,protocollo)) < 0)
	{
		perror("socket");
		exit(1);
	}
	return n;
}

/*
 * Effettua la connessione, utilizzata dal client
 */
void Connessione(int sockfd, struct sockaddr_in servaddr)
{
	int n;
	if ((n = connect(sockfd,(struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
	{
		perror("connect");
		exit(1);
	}
}

void AssegnaIndirizzo(int sockfd, struct sockaddr_in servaddr)
{
	int n;
	if ((n = bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0)
	{
		perror("bind");
		exit(1);
	}
}

void Ascolto(int sockfd, int lengthQueue)
{
	int n;
	if ((n = listen(sockfd, lengthQueue)) < 0)
	{
		perror("listen");
		exit(1);
	}
}

void ImpostaIndirizzoClient(int famiglia, char *addr, int porta, struct sockaddr_in *servaddr)
{
	struct hostent *host;
	servaddr->sin_family = famiglia;
	servaddr->sin_port = htons(porta);
	if ((host = gethostbyname2(addr,famiglia)) == NULL)
	{
		herror("Imposta Indirizzo Client");
		exit(1);
	}
	bcopy((char *) host->h_addr, (char *) &servaddr->sin_addr.s_addr, host->h_length);
}

void ImpostaIndirizzoAnyServer(int famiglia, int porta, struct sockaddr_in *servaddr)
{
	servaddr->sin_family = famiglia;
	servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr->sin_port = htons(porta);
}

int AccettazioneNuovaRichiesta(int sockfd)
{
	int n;
	if ((n = accept(sockfd, (struct sockaddr *) NULL, NULL)) < 0)
	{
		perror("accept");
		exit(1);
	}
	return n;
}

int AccettazioneNuovaRichiestaConcorrente(int sockfd, struct sockaddr_in *client, socklen_t *len)
{
	int n;
	if ((n = accept(sockfd, (struct sockaddr *) &client, len)) < 0)
	{
		perror("accept");
		exit(1);
	}
	return n;
}

ssize_t FullWrite(int fd, const void *buf, size_t count)
{
	size_t nleft;
	ssize_t nwritten;
	nleft = count;
	while(nleft > 0)
	{
		if ((nwritten = write(fd,buf,nleft)) < 0)
		{
			if (errno == EINTR)
				continue;
			else
			{
				perror("write");
				exit(1);
			}
		}
		nleft -= nwritten;
		buf += nwritten;
	}
	return nleft;
}

ssize_t FullRead(int fd, void *buf, size_t count)
{
	size_t nleft;
	ssize_t nread;
	nleft = count;
	while(nleft > 0)
	{
		if ((nread = read(fd,buf,nleft)) < 0)
		{
			if(errno == EINTR)
				continue;
			else
			{
				perror("read");
				exit(1);
			}
		}
		else if (nread == 0) //EOF
			break;
		nleft -= nread;
		buf += nread;
	}
	buf = 0;
	return nleft;
}

