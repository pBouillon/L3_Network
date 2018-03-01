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

/* signals */
#include <signal.h>

/* MACROS */
#define  BUFF_SIZE      256
#define  FORK_LIM       50
#define  LISTEN_BCKLOG  1
#define  NO_FLAG        0
#define  REQUIRED_ARGC  2
#define  TRUE           1


/*******************************************/
/* Serveur TCP qui renvoi une chaine de    */
/* caractere et la retourne en majuscule   */
/* Le numero de port est passe en argument */
/*******************************************/


int cli_desc[FORK_LIM] ;  /* descripteur de socket client */
int sock_desc ;  /* descripteur de socket client */

pid_t pids[FORK_LIM] ;
int fork_cpt ;

void close_tcp()
{
    int cpt ;
    for (cpt = 0; cpt < fork_cpt; ++cpt)
    {
        close (cli_desc[fork_cpt]) ;
        exit (EXIT_SUCCESS) ;
    }
    close (sock_desc) ;
}

void send_back (char* buff, int cli_desc)
{
    int cpt = 0 ;
    int nb_ret  ;

    /* manipulation de la chaine recue */
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
}

void set_mask ()
{
    struct sigaction sa ;

    sa.sa_handler = close_tcp ;
    sa.sa_flags   = 0 ;
    sigemptyset(&(sa.sa_mask)) ;
    sigaction(SIGINT, &sa, NULL) ;;
}

int main (int argc, char **argv)
{
    char buff[BUFF_SIZE] ; /* message recu et a envoyer */
    int nb_ret   ;  /* controle des valeurs des primitives */
    int err_ret  ;  /* controle du retour d'erreur */
    int sock_desc  ; /* descripteur de socket */
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

    /* preleve un client dans la file d'attente */
    memset (
        &cli_in,
        0,
        sizeof (cli_in)
    ) ;
    addrlen = sizeof (cli_in) ;

    serv_in.sin_addr.s_addr = htonl (INADDR_ANY) ;
    serv_in.sin_port   = htons (atoi(argv[1])) ;
    serv_in.sin_family = AF_INET ;

    /* liaison structure serveur/socket */
    set_mask() ;

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

    /* met TCP en attente */
    err_ret = listen (
        sock_desc,
        LISTEN_BCKLOG  // backlog size
    ) ;

    if (err_ret < 0) {
        perror ("listen: impossible d'ecouter sur le port\n") ;
        exit (EXIT_FAILURE) ;
    }

    fork_cpt = 0 ;
    while (TRUE)
    {
        if ((pids[fork_cpt++] = fork()) < 0)
	{
	    perror ("fork: impossible de fork\n") ;
            exit (EXIT_FAILURE) ;
	}
        // pere
	else if (pids[fork_cpt] == 1)
	{
	    if (fork_cpt > FORK_LIM)
	    {
	        close_tcp() ;
                exit (EXIT_SUCCESS) ;
	    }
	    continue ;
	}
        // fils
	else
	{
	    cli_desc[fork_cpt] = accept (
                sock_desc,
                (struct sockaddr *)&cli_in,
                &addrlen
            ) ;

	    /* reception de la chaine */
            nb_ret = read (
                cli_desc[fork_cpt],
                buff,
                BUFF_SIZE
            ) ;

	    if (nb_ret < 0)
            {
                perror ("read: Impossible de lire sur le port\n") ;
                exit (EXIT_FAILURE) ;
            }
            else
	    {
                printf ("%d caracteres recus\n", nb_ret) ;
            }
            send_back (buff, cli_desc[fork_cpt]) ;
	}
    }

    /* fermeture du serveur */
    close_tcp() ;
    return EXIT_SUCCESS ;
}
