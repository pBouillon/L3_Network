
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
/* client UDP qui envoie une chaine de caract�res  				  	     */
/* au serveur  UDP                              					     */
/* l'adresse IP du serveur et le num�ro de port sont pass�s en arguments */
/* r�cup�re la r�ponse du serveur et l'affiche 							 */
/*************************************************************************/

int main(int argc, char ** argv ) {

    int sd ; /* socket descriptor */
    struct sockaddr_in cli_addr ; /* structure socket pour le client */
    struct sockaddr_in ser_addr ; /* structure socket pour le serveur serveur  */
    struct in_addr adresse ; // structure pour ranger l'adresse IP sur 32 bits pour le protocole IP
    socklen_t addrlen ; /* pour stocker la longueur de la structure pass�e/demandee � l'OS*/
    int err; /* numero d'erreur rendu par certaines fonctions � tester !! */
    int nb ; /* nb octets pris en compte par le module UDP ou passes par UDP au client */

    /* buffer contenant la cha�ne � envoyer */
    char requete[256] = "la requete envoyee au serveur" ;
    /* buffer  o� sera rang�e la r�ponse */
    char reponse[256] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" ; 
  
/* adresse du serveur et num�ro de port en arguments de la commande */
    if (argc!=3) {
        printf("%s\n", "usage ./cli_udp @IPserveur #portserveur") ;
        exit(EXIT_FAILURE) ;
    }
/* 0. Initialisation des structures � 0 */
    /* initialisation  de la structure cli_addr  a` 0 */
    memset(&cli_addr, 0, sizeof(struct sockaddr_in)) ;
    /* initialisation  de la structure ser_addr  a` 0 */
    memset(&ser_addr, 0, sizeof(struct sockaddr_in)) ;
  

/* 1. creation du point de communication local */
    sd = socket(AF_INET, SOCK_DGRAM,0) ; 
    if(sd < 0) {
        perror("cli_udp: erreur ouverture socket") ;
        /* si on n'arrive pas � cr�er le point de communication inutile de continuer */
        exit(EXIT_FAILURE) ;
    }
  
/* 2. on ne nomme pas le point de communicationa 
     le syst�me d'exploitation le fera pour nous */
  
/* 3. remplissage des parties pertinentes (famaille, adresse, port) pour s'adresser au serveur */
    ser_addr.sin_family = AF_INET ;

    ser_addr.sin_port = htons(atoi(argv[2])) ;

    /* traduire l'adresse IP ASCII en une adresse 32 bits � mettre dans l'ent�te IP */
    err = inet_aton(argv[1], &adresse) ;
    if (err == 0) {
        perror("inet_aton: adresse IP non valide");
        /* si l'adresse n'est pas valide inutile de continuer */
        exit(EXIT_FAILURE) ;
    }
    ser_addr.sin_addr = adresse ;
  
	/* affichage de la chaine avant envoi */
    printf("cli_udp: je vais envoyer la chaine de caract�res: \"%s\" au serveur\n", requete) ;

/* 4. passer la cha�ne de caract�res compl�te � UDP  */
    err = sendto (
        sd,                           // socket descriptor
        requete,                      // message � envoyer
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

    if (err < 0) { // -1 si echec de l'envoi
        perror("envoie de la chaine impossible") ;
        exit(EXIT_FAILURE) ;
    } else {
    	printf("%d caract�res envoy�s\n", err) ; // taille du message envoy� si succ�s
    }
    

/* 6. recup�ration de la r�ponse du serveur dans le buffer reponse */
    err = recvfrom (
        sd,                           // socket descriptor
        reponse,                      // buffer pour stocker la r�ponse
        strlen(reponse),              // taille du message � stocker
        0,                            // flags
        (struct sockaddr *)&cli_addr, // adresse source 
        &addrlen                      // taille de l'adresse
    );

/* 7 tester le retour d'erreur et afficher le retour de la primitive */

    if (err < 0) { // renvoie -1 si il n'y a aucun message et que la socket est non-bloquante
        perror("impossible de r�cup�rer la r�ponse serveur") ;
        exit(EXIT_FAILURE) ;
    } else {
    	printf("%d caract�res re�us\n", err) ; // taille du message re�u si succ�s
    }


/* 8. affichage de  la r�ponse */
    printf("R�ponse serveur:\n%s\n", reponse) ;

/* 9. fermeture du point de communication */
    close (sd) ;
  
    return (EXIT_SUCCESS) ;
}
