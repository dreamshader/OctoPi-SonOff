// * **************************************************************************
// * A shutdown daemon
// * (C) 2017 Dirk Schanz, aka dreamshader
// * ==========================================================================
// * This shutdown daemon is a simple TCP/IP server listening on a
// * specific port for a shutdown message sent by a customized SONOFF
// * module.
// * The shutdown command is executed, after an acknowledge is sent back
// * to inform the SONOFF module that the command was received and now
// * will be executed.
// * The SONOFF module waits for a user defined time, before powering off
// * the line.
// * If something went wrong, e.g. the shutdown fails for some reason,
// * the server will send a CANCEL request to the SONOFF module. In this
// * case, the SONNOFF will NOT poweroff the line.
// * ==========================================================================
// *
// * Use makefile to compile it.
// *
// * ==========================================================================
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
// * You should have received a copy of the GNU General Public License
// * along with this program.  If not, see <http://www.gnu.org/licenses/>.
// * **************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>


//
// *************************************************************
// we hold options in this struct
// *************************************************************
//
struct _pgmArgs {
  short isDaemon;
  short port;
  short verbose;
  short dryrun;
};

#define DAEMON_NAME     "shutdown_d"
#define DAEMON_PORT     1717
// number of read attempts
#define NUMBER_OF_READS            3

#define MESSAGE_ACKNOWLEDGE    "ACK"
#define MESSAGE_CANCEL       "ABORT"
#define MESSAGE_SHUTDOWN      "HALT"

#define MSG_SHUTDOWN               3
#define MSG_INVAL                  4
#define MSG_UNKOWN                 5


// LOG_WARNING 
// LOG_CRIT 
// LOG_INFO 
// LOG_DEBUG 

//
// *************************************************************
// print out a simple help message
// *************************************************************
//
void usage( char* progName )
{
   fprintf(stderr, "Usage: %s [options] ", progName != NULL ? progName : "??" );
   fprintf(stderr, "where options are:\n");
   fprintf(stderr, "--port <listen port> ( or -p <listen port> )\n");
   fprintf(stderr, "  port for server to bind an listen (default=%d)\n", DAEMON_PORT);
   fprintf(stderr, "--dry             ( or -d )\n");
   fprintf(stderr, "  perform a dry run (no shutdown, default=0)\n");
   fprintf(stderr, "--verbose <level> ( or -v <level> )\n");
   fprintf(stderr, "  level is the loudness to talk(default=0)\n");
   fprintf(stderr, "--help            ( or -h / or -? )\n");
   fprintf(stderr, "  display this help\n");
   exit(0);
}

//
// *************************************************************
// Get command line options:
//
// --dry		( -t )
// --verbose <level>	( -v <level> )
// --help               ( -h / -? )
//
// *************************************************************
//
void get_arguments ( int argc, char **argv, struct _pgmArgs *pArgs )
{
  int next_option;

  /* valid short options letters */
  const char* const short_options = "p:g:rftd:v:h?";

  /* valid long options */
  const struct option long_options[] = {
    { "port",	        1, NULL, 'p' },
    { "dry",	        0, NULL, 'd' },
    { "verbose",	1, NULL, 'v' },
    { "help",		0, NULL, '?' },
    { NULL,		0, NULL,  0  }
  };

  if ( pArgs != NULL )
  {
    do
    {
      next_option = getopt_long (argc, argv, short_options,
                                     long_options, NULL);
      switch (next_option)
      {
        case 'v':
          pArgs->verbose = (short) atoi(optarg);
          break;
        case 'p':
          pArgs->port = (short) atoi(optarg);
          break;
        case 'd':
          pArgs->dryrun = (short) 1;
          break;
        case '?':
        case 'h':
          usage(argv[0]);
          break;
        case -1:    /* Done with options.  */
          break;
        default:
fprintf(stderr, "Unknown option >%c<\n", next_option);
          break;
      }
    } while( next_option != -1 );
  }
}

//
// *************************************************************
// signal handling stuff for daemon mode
// *************************************************************
//
typedef void (*sighandler_t)(int);

static sighandler_t handle_signal (int sig_nr, sighandler_t signalhandler) 
{
  struct sigaction neu_sig, alt_sig;
  neu_sig.sa_handler = signalhandler;
  sigemptyset (&neu_sig.sa_mask);
  neu_sig.sa_flags = SA_RESTART;

  if (sigaction (sig_nr, &neu_sig, &alt_sig) < 0)
  {
    return SIG_ERR;
  }

  return alt_sig.sa_handler;
}
//
// *************************************************************
// make program to run as a daemon
// *************************************************************
//
int daemonize( char* daemonName )
{
  int i;
  pid_t pid;
  char *dmnName;

  if( (dmnName = daemonName) == NULL )
  {
    dmnName = DAEMON_NAME;
  }


  if ((pid = fork ()) != 0)
  {
    if( pid < 0 )
    {
      return( errno );
    }
    else
    {
      exit( 0 );
    }
  }

  if (setsid() < 0)
  {
    return( errno );
  }

  handle_signal (SIGHUP, SIG_IGN);

  if ((pid = fork ()) != 0)
  {
    if( pid < 0 )
    {
      return( errno );
    }
    else
    {
      exit( 0 );
    }
  }

  chdir ("/");
  umask (0);
  for (i = sysconf (_SC_OPEN_MAX); i > 0; i--)
  {
    close (i);
  }


  openlog( dmnName, LOG_PID | LOG_CONS| LOG_NDELAY, LOG_LOCAL0 );

  return(1);
}

int parse_message( char message[], int dataLen )
{
    int retVal = MSG_UNKOWN;

    if( dataLen >= strlen(MESSAGE_SHUTDOWN) )
    {
        if( strncasecmp(message, MESSAGE_SHUTDOWN, strlen(MESSAGE_SHUTDOWN) ) == 0 )
        {
            retVal = MSG_SHUTDOWN;
        }
    }
    else
    {
        retVal = MSG_INVAL;
    }

    return( retVal );
}

void shutdownSystem( void )
{
     syslog(LOG_INFO, "Halting system");
//     execl( "/sbin/shutdown", "shutdown", "-h", "now", NULL );
     syslog(LOG_INFO, "/sbin/shutdown" );
}


//
// *************************************************************
//

int run_server( struct _pgmArgs *pArgs )
{
    char dataBuffer[4096];
    int sockfd, newsockfd;
    socklen_t clientLen;
    struct sockaddr_in serv_addr, clientAddr;
    int rcvLen, i;
    int one = 1;
    int retVal = 0;

syslog(LOG_DEBUG, "starting server ...");

    if( pArgs != (struct _pgmArgs*) NULL )
    {
        // create a INET socket ...
        if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        {
            syslog(LOG_ERR, "socket failed");
            retVal = errno;
        }
        else
        {
syslog(LOG_DEBUG, "socket ready ...");
            // enable reuse of port ...
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);

            // intialize structure with 0s
            memset(&serv_addr, '\0', sizeof(serv_addr));
            // net family is INET
            serv_addr.sin_family = AF_INET;
            // use any interface for the listener
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            // port must be in network-byte-order
            serv_addr.sin_port = htons(pArgs->port);

            // bin to local address
            if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            {
                syslog(LOG_ERR, "bind failed");
                retVal = errno;
            }
            else
            {
syslog(LOG_DEBUG, "socket bound ...");
                // set matching address-length for incoming connections
                clientLen = sizeof(clientAddr);
                // start listening on port
                // we allow to queue up to five incoming requests
                listen(sockfd,5);
                // accept a connection the request
                if( (newsockfd = accept(sockfd, (struct sockaddr *) &clientAddr, &clientLen)) < 0 )
                {
                    syslog(LOG_ERR, "accept failed");
                    retVal = errno;
                }
                else
                {
syslog(LOG_DEBUG, "incoming connection accepted ...");
                    // loop for several reads
                    for (i = 0; i < NUMBER_OF_READS; ++i) 
                    {
                        memset(dataBuffer,'\0', sizeof(dataBuffer));
                        // read data
                        if( (rcvLen = read(newsockfd,dataBuffer,sizeof(dataBuffer))) < 0 )
                        {
                            syslog(LOG_ERR, "read socket failed ... aborting!");
                            retVal = errno;
                        }
                        else
                        {
syslog(LOG_DEBUG, "got message ...");
                            if( parse_message( dataBuffer, rcvLen ) == MSG_SHUTDOWN )
                            {
syslog(LOG_DEBUG, "message was shutdown message ...");
                                if( write(newsockfd, MESSAGE_ACKNOWLEDGE, 
                                           strlen(MESSAGE_ACKNOWLEDGE)) < 0 )
                                {
                                    syslog(LOG_ERR, "write socket failed ... aborting!");
                                    retVal = errno;
                                    if( write(newsockfd, MESSAGE_CANCEL, 
                                                    strlen(MESSAGE_CANCEL)) < 0 )
                                    {
                                        syslog(LOG_ERR, "write socket failed ... aborting!");
                                        retVal = errno;
                                    }
                                }
                                else
                                {
syslog(LOG_DEBUG, "message acknoledged ...");
                                    shutdownSystem();
                                    syslog(LOG_ERR, "shutdown failed ... aborting!");
                                    retVal = errno;

                                    if( write(newsockfd, MESSAGE_CANCEL, 
                                               strlen(MESSAGE_CANCEL)) < 0 )
                                    {
                                        syslog(LOG_ERR, "write socket failed ... aborting!");
                                    }
                                }
                            }
                            else
                            {
                                syslog(LOG_ERR, "Unknown message ... ignored!");
                                retVal = -1;
                            }
                        }
                    }
                    // close client connection
                    close(newsockfd);
                }
                // close server socket
                close(sockfd);
            }
        }
    }
    else
    {
        retVal = -1;
    }

    return( retVal );
}


int main(int argc, char **argv)
{
    struct _pgmArgs callerArgs;
    int retVal = 0;

    callerArgs.port = DAEMON_PORT;
    callerArgs.isDaemon = 0;
    callerArgs.verbose = 0;
    callerArgs.dryrun = 0;

    get_arguments ( argc, argv, &callerArgs );

    if( !callerArgs.dryrun )
    {
        if( daemonize(NULL) )
        {
            callerArgs.isDaemon = 1;
            retVal = run_server( &callerArgs );
        }
        else
        {
            perror("cannot daemonize! Aborted ...");
            retVal = errno;
        }
    }
    else
    {
        usage( argv[0] );
        retVal = 0;
    }

     return(retVal);
}

