/* socket et bindings */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

/* sorties */
#include <stdio.h>

/* memset */
#include <string.h>

/* atoi */
#include <stdlib.h>

/* toupper */
#include <ctype.h>


/* taille du buffer */
#define  BUFF_SIZE  256


int main (int argc, char **argv)
{
    struct sockaddr_in serv_addr ; /* structure serveur */
    struct sockaddr_in cli_addr  ; /* structure client  */
    struct in_addr addr ; /* structure stockage IP */
    char buff[BUFF_SIZE] ; /* buffer pour reception/reponse  */
    socklen_t addr_len ;   /* taille de strucure sockaddr_in */
    int cpt  ; /* compteur pour parsing du buffer */
    int ret  ; /* pour verification des retours   */
    int sock ; /* descripteur de socket */

    /* verification des arguments */
    if (argc != 2)
    {
        printf ("usage: %s port\n", argv[0]) ;
        exit (EXIT_FAILURE) ;
    }

    /* creation de la socket */
    sock = socket (AF_INET, SOCK_DGRAM, 0) ;
    if (sock < 0)
    {
        perror ("socket: impossible de creer la socket\n") ;
        exit (EXIT_FAILURE) ;
    }

    /* initialisation de la structure serveur */
    memset (
        &serv_addr,
        0,
        sizeof (serv_addr)
    ) ;

    serv_addr.sin_family = AF_INET ;
    serv_addr.sin_port   = htons (atoi(argv[1])) ;

    addr.s_addr = INADDR_ANY ;
    serv_addr.sin_addr = addr ;

    /* initialisation de la structure client */
    memset (
        &cli_addr,
        0,
        sizeof (cli_addr)
    ) ;

    /* attachement de la structure a la socket */
    addr_len = sizeof (serv_addr) ;
    ret = bind (
        sock,
        (struct sockaddr *)&serv_addr,
        addr_len
    ) ;
    if (ret < 0)
    {
        perror ("bind: impossible de bind\n") ;
        exit (EXIT_FAILURE) ;
    }

    /* reception du paquet (message) client */
    ret = recvfrom (
        sock,
        buff,
        sizeof (buff) + 1,
        0,
        (struct sockaddr *)&cli_addr,
        &addr_len
    ) ;
    if (ret < 0)
    {
        perror ("recvfrom: impossible d'ecouter le port\n") ;
        exit (EXIT_FAILURE) ;
    }

    printf ("%d caracteres recus :\n\t%s\n", ret, buff) ;

    /* conversion du message en majuscule */
    cpt = 0 ;
    while (buff[cpt])
    {
        buff[cpt] = toupper (buff[cpt]) ;
        ++cpt ;
    }
    printf ("Envoi de: \n\t%s\n", buff) ;

    /* envoi du message en majuscule */
    addr_len = sizeof (serv_addr) ;
    ret = sendto (
        sock,
        buff,
        strlen (buff) + 1,
        0,
        (struct sockaddr *)&cli_addr,
        addr_len
    ) ;
    if (ret < 0)
    {
        perror ("sendto: impossible d'envoyer le message\n") ;
        exit (EXIT_FAILURE) ;
    }

    printf ("Envoi de %d caracteres reussi\n", ret) ;

    /* fermeture de la socket */
    close (sock) ;
    exit (EXIT_SUCCESS) ;
}
