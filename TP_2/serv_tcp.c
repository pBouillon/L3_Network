/* sockaddr, sockaddr_in, etc. */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

/* close */
#include <unistd.h>

/* perror et printf */
#include <errno.h>
#include <stdio.h>

/* memset */
#include <string.h>

/* EXIT constants */
#include <stdlib.h>

/* buffer */
#include <ctype.h>

/* MACROS */
#define  BUFF_SIZE      256
#define  LISTEN_BCKLOG  1
#define  NO_FLAG        0
#define  REQUIRED_ARGC  2


/*******************************************/
/* Serveur TCP qui renvoi une chaine de    */
/* caractere et la retourne en majuscule   */
/* Le numero de port est passe en argument */
/*******************************************/


int main (int argc, char **argv)
{
    char buff[BUFF_SIZE] ; /* message recu et a envoyer */
    int cpt ;       /* compteur */
    int cli_desc ;  /* descripteur de socket client */
    int err_ret  ;  /* controle du retour d'erreur */
    int nb_ret   ;  /* controle des valeurs des primitives */
    int sock_desc ; /* descripteur de socket */
    struct sockaddr_in cli_in  ; /* structure client */
    struct sockaddr_in serv_in ; /* structure serveur */
    socklen_t addrlen ;          /* taille de la struct sockaddr_in */

    /* controle des parametres */
    if (argc != REQUIRED_ARGC)
    {
        printf ("usage: %s port\n", argv[0]) ;
        exit (EXIT_FAILURE) ;
    }

    /* creation de la socket */
    sock_desc = socket (AF_INET, SOCK_STREAM, NO_FLAG) ;
    if (sock_desc < 0)
    {
        perror ("socket: impossible de creer la socket\n") ;
        exit (EXIT_FAILURE) ;
    }

    /* instanciation de la structure serveur */
    memset (
        &serv_in,
        0,
        sizeof (struct sockaddr_in)
    ) ;
    serv_in.sin_addr.s_addr = htonl (INADDR_ANY) ;
    serv_in.sin_port   = htons (atoi(argv[1])) ;
    serv_in.sin_family = AF_INET ;

    /* liaison structure serveur/socket */
    addrlen = sizeof (serv_in) ;

    err_ret = bind (
        sock_desc,
        (struct sockaddr *)&serv_in,
        addrlen
    ) ;

    if (err_ret < 0 )
    {
        perror ("bind: echec\n") ;
        exit (EXIT_FAILURE) ;
    }

    /* ecoute */
    err_ret = listen (
        sock_desc,
        LISTEN_BCKLOG
    ) ;

    if (err_ret < 0) {
        perror ("listen: impossible d'ecouter sur le port\n") ;
        exit (EXIT_FAILURE) ;
    }

    /* acceptation de la connection entrante */
    memset (
        &cli_in,
        0,
        sizeof (cli_in)
    ) ;

    addrlen = sizeof (cli_in) ;
    cli_desc = accept (
        sock_desc,
        (struct sockaddr *)&cli_in,
        &addrlen
    ) ;

    /* reception de la chaine */
    nb_ret = read (
        cli_desc,
        buff,
        BUFF_SIZE
    ) ;

    if (nb_ret < 0)
    {
        perror ("read: Impossible de lire sur le port\n") ;
        exit (EXIT_FAILURE) ;
    } else {
        printf ("%d caracteres recus\n", nb_ret) ;
    }

    /* manipulation de la chaine recue */
    cpt = 0 ;
    while (buff[cpt])
    {
        buff[cpt] = toupper (buff[cpt]) ;
        ++cpt ;
    }
    printf ("Envoi de: \n\t%s\n", buff) ;

    /* retour de la chaine */
    nb_ret = write (
        cli_desc,
        buff,
        strlen (buff) + 1
    ) ;

    if (nb_ret < 0)
    {
        perror ("write: envoi impossible\n") ;
        exit (EXIT_FAILURE) ;
    } else {
        printf ("%d caracteres envoyes\n", nb_ret) ;
    }

    /* fermeture du serveur */
    close (cli_desc)  ;
    close (sock_desc) ;
    return EXIT_SUCCESS ;
}
