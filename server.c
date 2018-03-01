#include "mywrapper.h"
#include <string.h>

#define PORTA 3000
#define N_RIGHE 4
#define N_COLONNE 4
#define N_BARCHE 4
#define CLIENT_QUEUE 100

struct pacchetto {
	int riga;
	int colonna;
	int esito_giocata;
	int esito_partita;
};

int verifica_input(int,int);

int main(int argc, char *argv[])
{
	int listenfd, connfd, n, turno, t, i, j, num_navi = N_BARCHE;
	int enabled = 1;
	pid_t pid;
	struct sockaddr_in my_addr;
	struct pacchetto package;
	int campo[N_RIGHE][N_COLONNE];
	listenfd = CreaSocket(AF_INET, SOCK_STREAM, 0);
	ImpostaIndirizzoAnyServer(AF_INET,PORTA,&my_addr);
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0)
	{
		perror("setsockopt");
		exit(1);
	}
	AssegnaIndirizzo(listenfd, my_addr);
	Ascolto(listenfd, CLIENT_QUEUE);
	while(1)
	{
		connfd = AccettazioneNuovaRichiesta(listenfd);
		if((pid = fork()) < 0) //Errore
		{
			perror("fork");
			close(connfd);
		}
		if(pid == 0) //Figlio
		{
			close(listenfd);
			for(t=0;t<N_RIGHE;t++)
				for(j=0;j<N_COLONNE;j++)
					campo[t][j] = 0;
			for(t=0;t<N_BARCHE;t++)
			{
				while(getchar() != '\n');
				fprintf(stdout,"indica la posizione della nave sul campo di battaglia:");
				fscanf(stdin,"%d%d",&i,&j);
				if(verifica_input(i,j) == 0)
					t -= 1;
				else if(campo[i][j] == 0)
					campo[i][j] = 1;
				else
				{
					fprintf(stdout,"La posizione e' gia' occupata\n");
					t -= 1;
				}
			}
			package.esito_giocata = 0;
			package.esito_partita = 0;
			turno = 0;
			FullWrite(connfd,&turno,sizeof(int));
			do
			{
				if(turno == 0) //turno Client
				{
					while((n = FullRead(connfd,&package,sizeof(package))) > 0);
					if(campo[package.riga][package.colonna])
					{
						campo[package.riga][package.colonna] = 0;
						package.esito_giocata = 1;
						num_navi -= 1;
						if(num_navi <= 0) //sono state abbattute tutte le navi
							package.esito_partita = 1;
					}
					else
						package.esito_giocata = 0;
					FullWrite(connfd,&package,sizeof(package));
					turno = 1;
				}
				else //turno Server
				{
					do
					{
						fprintf(stdout,"Quale posizione vuoi colpire? ");
						fscanf(stdin,"%d%d",&package.riga,&package.colonna);
					} while(verifica_input(package.riga,package.colonna) == 0);
					FullWrite(connfd,&package,sizeof(package));
					while((n = FullRead(connfd,&package,sizeof(package))) > 0);
					if(package.esito_giocata)
						fprintf(stdout,"Centrata in pieno\n");
					else
						fprintf(stdout,"Mancata, ritenta al tuo prossimo turno\n");
					turno = 0;
				}
				if(package.esito_partita != 1)
					FullWrite(connfd,&turno,sizeof(int));
			} while(package.esito_partita == 0);
			close(connfd);
			exit(0);
		}
		close(connfd);
	}
	exit(0);
}

int verifica_input(int i, int j)
{
	if((i<0 || i>N_RIGHE) || (j<0 || j>N_COLONNE))
	{
		fprintf(stdout,"!!!! ATTENZIONE !!!!\nNumero max di righe: %d\nNumero max di colonne: %d\n", N_RIGHE-1,N_COLONNE-1);
		return 0;
	}
	return 1;
}	
