/**
 * Copyright (c) 2014, 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <getopt.h>
#include "debug.h"
#include "conn_mgr.h"
#include "nl_loop.h"
#include "cnss_dp.h"

#define  DAEMONIZE      0x1
#define  LOG_FILE       0x2
#define  SYS_LOG        0x4

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_arr) (sizeof(_arr) / sizeof((_arr)[0]))
#endif

extern int wsvc_debug_level;
static char *progname = NULL;

static inline int wlan_svc_aquire_wake_lock(void)
{
    wsvc_printf_dbg("Aquiring wake lock");

    return system("echo wlan_services > /sys/power/wake_lock");
}

static inline int wlan_svc_release_wake_lock(void)
{
    wsvc_printf_dbg("Releasing wake lock");

    return system("echo wlan_services > /sys/power/wake_unlock");
}

static void wlan_svc_restart(unsigned short type, void *user_data)
{
    wsvc_printf_dbg("Restarting WLAN: %u, %p", type, user_data);

    wlan_svc_aquire_wake_lock();

    if (conn_mgr_restart_wlan(user_data))
        wsvc_printf_err("Restart WLAN failed!");

    wlan_svc_release_wake_lock();

    return;
}

static void wlan_svc_shutdown(unsigned short type, void *user_data)
{
    wsvc_printf_dbg("Shutdown WLAN: %u, %p", type, user_data);

    wlan_svc_aquire_wake_lock();

    if (conn_mgr_stop_wlan(user_data))
        wsvc_printf_err("WLAN Shutdown failed!");

    wlan_svc_release_wake_lock();

    return;
}

static struct ind_handlers {
    unsigned short ind;
    nl_loop_ind_handler handler;
    void *user_data;
} ind_handlers [] = {
    {
        .ind = WLAN_SVC_FW_CRASHED_IND,
        .handler = wlan_svc_restart,
        .user_data = NULL,
    },
    {
        .ind = WLAN_SVC_LTE_COEX_IND,
        .handler = wlan_svc_restart,
        .user_data = NULL,
    },
    {
        .ind = WLAN_SVC_WLAN_AUTO_SHUTDOWN_IND,
        .handler = wlan_svc_shutdown,
        .user_data = NULL,
    },
};

static void usage(void)
{
    fprintf(stderr, "usage: %s [options]\n"
            "   -n, --nodaemon  do not run as a daemon\n"
            "   -d              show more debug messages (-dd for even more)\n"
#ifdef CONFIG_DEBUG_FILE
            "   -f <path/file>  Log output to file\n"
#endif
#ifdef CONFIG_DEBUG_SYSLOG
            "   -s              Log output to syslog\n"
#endif
            "       --help      display this help and exit\n"
            , progname);
    exit(EXIT_FAILURE);
}

static void wlan_service_sighandler(int signum)
{
    wsvc_printf_info("Caught Signal: %d", signum);
    nl_loop_terminate();

    return;
}

static int wlan_service_setup_sighandler(void)
{
    struct sigaction sig_act;

    memset(&sig_act, 0, sizeof(sig_act));
    sig_act.sa_handler = wlan_service_sighandler;
    sigemptyset(&sig_act.sa_mask);

    sigaction(SIGQUIT, &sig_act, NULL);
    sigaction(SIGTERM, &sig_act, NULL);
    sigaction(SIGINT, &sig_act, NULL);
    sigaction(SIGHUP, &sig_act, NULL);

    return 0;
}

int main(int argc, char *argv[])
{
    int c;
    int i;
    int flag = DAEMONIZE;
    static struct option options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"nodaemon", no_argument, NULL, 'n'},
        {0, 0, 0, 0}
    };
    void *cm_handle = NULL;
    char *log_file = NULL;

    progname = argv[0];

    while (1) {
        c = getopt_long(argc, argv, "hdnf:s", options, NULL);

        if (c < 0)
            break;

        switch (c) {
        case 'n':
            flag &= ~DAEMONIZE;
            break;
        case 'd':
#ifdef CONFIG_DEBUG
            wsvc_debug_level++;
#else
            wsvc_printf_err("Debugging disabled, please build with -DDEBUG");
            exit(EXIT_FAILURE);
#endif
            break;
#ifdef CONFIG_DEBUG_FILE
        case 'f':
            log_file = optarg;
            flag |= LOG_FILE;
            break;
#endif /* CONFIG_DEBUG_FILE */
#ifdef CONFIG_DEBUG_SYSLOG
        case 's':
            flag |= SYS_LOG;
            break;
#endif /* CONFIG_DEBUG_SYSLOG */

        case 'h':
        default:
            usage();
            break;
        }
    }

    if (optind < argc)
        usage();

    wsvc_debug_init();

    if (flag & SYS_LOG)
        wsvc_debug_open_syslog();
    else if (flag & LOG_FILE)
        wsvc_debug_open_file(log_file);

    wlan_service_setup_sighandler();

    if (flag & DAEMONIZE && daemon(0, 0)) {
        wsvc_perror("daemon");
        exit(EXIT_FAILURE);
    }

    cm_handle = conn_mgr_register();
    if (cm_handle == NULL) {
        wsvc_printf_err("Not able to register with conn_mgr!");
        goto out;
    }

    if (nl_loop_init()) {
        wsvc_printf_err("Failed to init nl_loop");
        goto out;
    }

    for (i = 0; i < ARRAY_SIZE(ind_handlers); i++) {
        if (nl_loop_register(ind_handlers[i].ind, ind_handlers[i].handler,
                    cm_handle)) {
            wsvc_printf_err("Failed to register: %d, %x", i,
                    ind_handlers[i].ind);
            goto register_fail;
        }
    }

    if (wlan_dp_service_start()) {
        wsvc_printf_err("Failed to start wlan datapath service");
        goto register_dp_fail;
    }

    nl_loop_run();

register_dp_fail:
    wlan_dp_service_stop();

register_fail:
    while (--i >= 0)
        nl_loop_unregister(ind_handlers[i].ind);

    nl_loop_deinit();

out:
    if (cm_handle)
        conn_mgr_unregister(cm_handle);

    wsvc_debug_close_syslog();
    wsvc_debug_close_file();

    exit(EXIT_SUCCESS);
}
