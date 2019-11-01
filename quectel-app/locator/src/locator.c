#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ql_oe.h>
#include "ql_cell_locator.h"

void usage()
{
    printf("usage: test_locator_api [options]\n"
            "      before test locator, You must confirm that you are connected.\n"
            "options:\n"
            " -s --server, Set query server domain or ip, default is www.queclocator.com.\n"
            " -p --port, Connect server use given port, default is 80.\n"
            " -t --token, Used to verify client privilege, length must be 16 bytes\n"
            " -o --timeout, value of query timeout, default is 10 seconds\n"
            " eg: ./locator -s www.queclocator.com -p 80 -t xxxxxxxxxxxxxxxx -o 10\n"
          );
}

int main(int argc, char **argv)
{
    int opt, opt_index;
    int ret, i;
    ql_cell_resp resp;
    char server[255] = "www.queclocator.com", token[32] = {0};
    unsigned short port = 80, timeout = 10;

    static struct option long_option[] = {
		{"server", required_argument, NULL, 's'},
		{"port", required_argument, NULL, 'p'},
		{"token", required_argument, NULL, 't'},
		{"timeout", required_argument, NULL, 'o'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0},
    };
    
    while((opt = getopt_long(argc, argv, "s:p:t:o:", long_option, &opt_index)) != -1)
    {
        switch(opt)
        {
            case 0:
				break;
            case 'h':
                usage();
                exit(0);
                break;
            case 's':
            {
                if(optarg != NULL)
                {
                    strncpy(server, optarg, sizeof(server));
                }
                else
                {
                    strncpy(server, "www.queclocator.com", sizeof(server));
                }
            }
            break;
            case 'p':
            {
                if(optarg != NULL)
                {
                    port = strtoul(optarg, NULL, 0);
                }
                else
                {
                    printf("port is missing, use default 80\n");
                    port = 80;
                }
            }
            break;
            case 't':
            {
                printf("tttttttttttttttttt%s\n", optarg);
                if(optarg != NULL)
                {
                    strncpy(token, optarg, sizeof(token));
                }
                else
                {
                    printf("token is missing.\n");
                    usage();
                    exit(0);
                }
            }
            break;
            case 'o':
            {
                if(optarg != NULL)
                {
                    timeout = strtoul(optarg, NULL, 0);
                }
                else
                {
                    printf("timeout is missing, use default 10\n");
                    timeout = 10;
                }
                
            }
        }
    }
    if(optind != argc) {
		fprintf(stderr, "Expected argument after options\n"); 
		usage();
		exit(1);
	}

    if(server[0] == 0 || strlen(server) == 0)
    {
        printf("server must be not null.\n");
        usage();
        return -1;
    }

    if(token[0] == 0 || strlen(token) == 0)
    {
        printf("token must be not null.\n");
        usage();
        return -1;
    }

    ret = ql_cell_locator_init();
    if(ret)
    {
        printf("ql_cell_locator_init error.\n");
        return -1;
    }

    ret = ql_cell_locator_set_server(server, port);
    if(ret)
    {
        printf("ql_cell_locator_set_server error.\n");
        return -1;
    }

    ret = ql_cell_locator_set_timeout(timeout);
    if(ret)
    {
        printf("ql_cell_locator_set_timeout error.\n");
        return -1;
    }

    ret = ql_cell_locator_set_token(token, strlen(token));
    if(ret)
    {
        printf("ql_cell_locator_set_token error.\n");
        return -1;
    }
    /*the frequency of access to the service is 10 seconds, so we can see  'perform fast, retry later'*/
    for(i = 0; i < 15; i++)
    {
        ret = ql_cell_locator_perform(&resp);
        if(ret)
        {
            printf("err_code: %d, err_msg: %s\n", resp.err.err_code, resp.err.err_msg);
        }
        else
        {
            printf("lon: %f, lat: %f, accuracy: %d\n", resp.lon, resp.lat, resp.accuracy);
        }
        sleep(1);
    }
    
    return 0;
}
