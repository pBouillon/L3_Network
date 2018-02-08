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


/* Macros */
#define  BUFF_SIZE      256
#define  EXPECTED_ARGC  3


/****************************************/
/* Client TCP, envoyant une chaine de   */
/* caracteres au serveur et affichant   */
/* la reponse de ce dernier             */
/* l'IP du serveur et le numero de port */
/* sont passes en argument              */
/****************************************/


/****************************************************/
/* Pour le message client:                          */
/* Le message sera envoye lors de la 4eme trame TCP */
/* Il sera acquite lors de la trame suivante (5eme) */
/*                                                  */
/* Pour la reponse:                                 */
/* Le retour ECHO sera envoye lors de la 6eme trame */
/* Son acquitement sera lors de la 7eme trame       */
/****************************************************/


int main (int argc, char **argv)
{
    struct sockaddr_in serv_in  ; /* structure serveur */
    struct in_addr     ser_addr ; /* range l'adresse IP sur 32b*/
    socklen_t addrlen ;           /* stocke la taille de la structure */

    char message[BUFF_SIZE]       /* message a envoyer */
        = "la requete a envoyer au serveur." ;
    char buff_retour[BUFF_SIZE]   /* retour du serveur */
        = "_______________________________________" ;

    int  sock_desc ; /* descripteur de socket */
    int  nb_ret  ;   /* recupere les valeurs remarquables des primitives*/
    int  err_ret ;   /* recupere le retour d'erreur */


    /* verification des arguments */
    if (argc != EXPECTED_ARGC)
    {
        printf (
            "usage: ./%s @IP_serv nb_port\n",
            argv[0]
        ) ;
        exit (EXIT_FAILURE) ;
    }

    /* initialisation des structures */
    memset (
        &serv_in,
        0,
        sizeof (struct sockaddr_in)
    ) ;

    /* creation de la socket */
    sock_desc = socket (AF_INET, SOCK_STREAM, 0) ;
    if (sock_desc < 0)
    {
        perror ("socket: impossible de creer la socket\n") ;
        exit (EXIT_FAILURE) ;
    }

    /* initialisation de la structure serveur */
    serv_in.sin_family = AF_INET ;
    serv_in.sin_port   = htons (atoi(argv[2])) ;

    /* conversion de l'IP en ASCII sur 32b */
    err_ret = inet_aton (argv[1], &ser_addr) ;
    if (err_ret < 0)
    {
        perror ("inet_aton: addresse IP invalide\n") ;
        exit (EXIT_FAILURE) ;
    }
    serv_in.sin_addr = ser_addr ;

    /* affichage intermediaire */
    printf (
        "Client TCP -- envoi de:\n\t%s\n",
        message
    ) ;

    /* connection au serveur */
    addrlen = sizeof (serv_in) ;
    err_ret = connect (
        sock_desc,
        (struct sockaddr*)&serv_in,
        addrlen
    ) ;

    if (err_ret < 0)
    {
        perror ("connect: impossible d'etablir la connection\n") ;
        exit (EXIT_FAILURE) ;
    }

    /* envoie de la chaine */
    nb_ret = write (
        sock_desc,
        message,
        strlen (message) + 1
    ) ;

    if (nb_ret < 0)
    {
        perror ("write: impossible d'envoyer le message\n") ;
        exit (EXIT_FAILURE) ;
    } else {
        printf ("%d caracteres envoyes\n", nb_ret) ;
    }

    /* reception de la chaine renvoyee */
    nb_ret = read (
        sock_desc,
        buff_retour,
        BUFF_SIZE
    ) ;

    if (nb_ret < 0)
    {
        perror ("read: impossible de lire le retour serveur\n") ;
        exit (EXIT_FAILURE) ;
    } else {
        printf ("%d caracteres recus.\n", nb_ret) ;
    }

    /* affichage de la chaine recue */
    printf ("Recu: \n\t%s\n", buff_retour) ;

    /* fermeture du programme client */
    close (sock_desc) ;
    return EXIT_SUCCESS ;
}
