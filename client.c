#include "mywrapper.h"
#include <string.h>

#define PORTA 3000
#define N_RIGHE 4
#define N_COLONNE 4
#define N_BARCHE 4

struct pacchetto {
	int riga;
	int colonna;
	int esito_giocata;
	int esito_partita;
};

int verifica_input(int,int);

int main(int argc, char *argv[])
{
	int sockfd, n, turno, t, i, j, num_navi = N_BARCHE;
	struct sockaddr_in servaddr;
	struct pacchetto package;
	int campo[N_RIGHE][N_COLONNE];
	if(argc != 2)
	{
		fprintf(stdout, "usage: %s <IP-address>\n",argv[0]);
		exit(1);
	}
	for(t=0;t<N_RIGHE;t++)
		for(j=0;j<N_COLONNE;j++)
			campo[t][j] = 0;
	sockfd = CreaSocket(AF_INET, SOCK_STREAM, 0);
	ImpostaIndirizzoClient(AF_INET, argv[1], PORTA, &servaddr);
	Connessione(sockfd, servaddr);
	for(t=0;t<N_BARCHE;t++)
	{
		fprintf(stdout,"indica la posizione della nave sul campo di battaglia: ");
		fscanf(stdin,"%d%d",&i,&j);
		if(verifica_input(i,j) == 0)
			t -= 1;
		else if(campo[i][j] == 0)
			campo[i][j] = 1;
		else
		{
			fprintf(stdout,"La posizione e gia' occupata\n");
			t -= 1;
		}
	}
	package.esito_giocata = 0;
	package.esito_partita = 0;
	while((n = FullRead(sockfd,&turno,sizeof(int))) > 0);
	do
	{
		if(turno == 0) //turno Client
		{
			do
			{
				fprintf(stdout,"Quale posizione vuoi colpire? ");
				fscanf(stdin,"%d%d",&package->riga,&package->colonna);
			} while(verifica_input(package.riga, package.colonna) == 0);
			FullWrite(sockfd,&package,sizeof(package));
			while((n = FullRead(sockfd,&package,sizeof(package))) > 0);
			if(package.esito_giocata)
				fprintf(stdout,"Centrata in pieno\n");
			else
				fprintf(stdout,"Mancata, ritenta al tuo prossimo turno\n");
		}
		else
		{
			while((n = FullRead(sockfd, &package,sizeof(package))) > 0);
			if(campo[package.riga][package.colonna])
			{
				campo[package.riga][package.colonna] = 0;
				package.esito_giocata = 1;
				num_navi -= 1;
				if(num_navi <= 0) //sono state abbattute tutte le navi, fine partita
					package.esito_partita = 1;
			}
			else
				package.esito_giocata = 0;
			FullWrite(sockfd,&package,sizeof(package));
		}
		if(package.esito_partita != 1)
			while((n = FullRead(sockfd, &turno, sizeof(int))) > 0);
	} while(package.esito_partita == 0);
	if(num_navi == 0)
		fprintf(stdout,"############### YOU LOSE ###############\n");
	else
		fprintf(stdout,"############## YOU WIN ###############\n");
	exit(0);
}

int verifica_input(int i, int j)
{
	if((i < 0 || i > N_RIGHE) || (j < 0 || j > N_COLONNE))
	{
		fprintf(stdout,"!!!! ATTENZIONE !!!!\nNumero max di righe: %d\nNumero max di colonne: %d\n",N_RIGHE-1,N_COLONNE-1);
		return 0;
	}
	return 1;
}
