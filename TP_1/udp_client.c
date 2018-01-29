
/* include pour sockaddr, sockadd_rin, socket, bind, sendto  inet_aton et htons */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* include pour close */
#include <unistd.h>

/* include pour perror*/
#include <stdio.h>
#include <errno.h>

/* include pour memset*/
#include <string.h>

/* include pour exit */
#include <stdlib.h>


/*************************************************************************/
/* client UDP qui envoie une chaine de caracteres                        */
/* au serveur UDP                                                        */
/* l'adresse IP du serveur et le numero de port sont passes en arguments */
/* recupere la reponse du serveur et l'affiche                           */
/*************************************************************************/

int main (int argc, char ** argv ) {

    int sd ; 		  	  /* socket descriptor */
    struct sockaddr_in cli_addr ; /* structure socket pour le client */
    struct sockaddr_in ser_addr ; /* structure socket pour le serveur serveur */
    struct in_addr adresse ;      /* structure pour ranger l'adresse IP sur 32 bits pour le protocole IP */
    socklen_t addrlen ;           /* pour stocker la longueur de la structure passee/demandee a l'OS */
    int err ; 			  /* numero d'erreur rendu par certaines fonctions a tester */
    int nb  ; 			  /* nb octets pris en compte par le module UDP ou passes par UDP au client */

    /* buffer contenant la chaine a envoyer */
    char requete[256] = "la requete envoyee au serveur" ;
    /* buffer  ou sera rangee la reponse */
    char reponse[256] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" ;

/* adresse du serveur et numero de port en arguments de la commande */
    if (argc != 3) {
        printf ("%s\n", "usage ./cli_udp @IPserveur #portserveur") ;
        exit (EXIT_FAILURE) ;
    }
/* 0. Initialisation des structures a 0 */
    /* initialisation  de la structure cli_addr a 0 */
    memset (
	&cli_addr,
	0,
	sizeof (struct sockaddr_in)
    ) ;
    /* initialisation  de la structure ser_addr a 0 */
    memset (
	&ser_addr,
	0,
	sizeof (struct sockaddr_in)
    ) ;

/* 1. creation du point de communication local */
    sd = socket (AF_INET, SOCK_DGRAM,0) ;
    if (sd < 0) {
        perror ("cli_udp: erreur ouverture socket") ;
        /* si on n'arrive pas a creer le point de communication inutile de continuer */
        exit (EXIT_FAILURE) ;
    }

/* 2. on ne nomme pas le point de communication
     le systeme d'exploitation le fera pour nous */

/* 3. remplissage des parties pertinentes (famaille, adresse, port) pour s'adresser au serveur */
    ser_addr.sin_family = AF_INET ;
    ser_addr.sin_port = htons(atoi(argv[2])) ;

    /* traduire l'adresse IP ASCII en une adresse 32 bits a mettre dans l'entete IP */
    err = inet_aton(argv[1], &adresse) ;
    if (err == 0) {
        perror ("inet_aton: adresse IP non valide") ;
        /* si l'adresse n'est pas valide inutile de continuer */
        exit (EXIT_FAILURE) ;
    }
    ser_addr.sin_addr = adresse ;

	/* affichage de la chaine avant envoi */
    printf (
	"cli_udp: je vais envoyer la chaine de caracteres: \"%s\" au serveur\n",
	requete
    ) ;

/* 4. passer la chaine de caracteres complete a UDP  */
    addrlen = sizeof(ser_addr) ;
    nb = sendto (
        sd,                           // socket descriptor
        requete,                      // message a envoyer
        sizeof(requete),              // taille du message
        0,                            // flags
        (struct sockaddr *)&ser_addr, // adresse de destination
        addrlen                       // taille de l'adresse
    ) ;

/*5. tester le retour d'erreur */
    /* la primitive rend une valeur qui a deux significations :
    	- un code d'erreur
    	- une autre valeur
    afficher la valeur lorsque ce n'est pas un code d'erreur sinon sortir */

    if (nb < 0) { // -1 si echec de l'envoi
        perror ("envoie de la chaine impossible") ;
        exit (EXIT_FAILURE) ;
    } else {
    	printf ("%d caracteres envoyes\n", nb) ; // taille du message envoye si succes
    }

/* 6. recuperation de la reponse du serveur dans le buffer reponse */
    nb = recvfrom (
        sd,                           // socket descriptor
        reponse,                      // buffer pour stocker la reponse
        sizeof(reponse),              // taille du message a stocker
        0,                            // flags
        (struct sockaddr *)&cli_addr, // adresse source
        &addrlen                      // taille de l'adresse
    ) ;

/* 7 tester le retour d'erreur et afficher le retour de la primitive */
    if (nb < 0) { // -1 si il n'y a aucun message et que la socket est non-bloquante
        perror ("impossible de recuperer la reponse serveur") ;
        exit (EXIT_FAILURE) ;
    } else {
    	printf ("%d caracteres recus\n", nb) ; // taille du message recu si succes
    }


/* 8. affichage de  la reponse */
    printf ("Reponse serveur:\n%s\n", reponse) ;

/* 9. fermeture du point de communication */
    close (sd) ;
    return (EXIT_SUCCESS) ;
}
