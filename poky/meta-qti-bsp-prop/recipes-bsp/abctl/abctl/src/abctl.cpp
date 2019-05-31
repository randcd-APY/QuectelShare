/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "libabctl.h"

#define VERSION_STR         ("v1.0\n")
#define UID_SYSTEM          (1000)    /* UID for system user. */
#define GID_DISK            (6)       /* GID for disk group.  */

typedef struct {
   char *option_string;
   unsigned int opt_str_len;
   int  (*action_func)(void *param);
   int  is_param_needed;
} opt_func_map;

static const char *slots[] = {(const char *)"_a", (const char *)"_b"};

static void usage()
{
   printf("abctl  -  control A/B boot for the device.\n");
   printf("   --help:\n");
   printf("         prints this help.\n");
   printf("   --version:\n");
   printf("         prints the program version string.\n");
   printf("   --set_active <slot-number>:\n");
   printf("         sets the slot specified by <slot-number> as active.\n");
   printf("   --boot_slot:\n");
   printf("         prints the current slot from which device booted up.\n");
   printf("   --set_success:\n");
   printf("         sets the success flag for slot from which device booted up.\n");
   printf("   --set_unbootable <slot-number>:\n");
   printf("         sets the unbootable flag for slot specified by <slot-number>.\n");
   printf("   --dbg: <0|1>\n");
   printf("         When set to '1', provides verbose prints during operations.\n");

   printf("\n");
}

static int print_version(void *p)
{
   printf("Version: %s\n", VERSION_STR);
   return 0;
}

static int print_help(void *p)
{
   usage();
   return 0;
}

static int do_get_boot_slot(void *p)
{
   int index = libabctl_getBootSlot();

   switch(index) {
      case 0:
      case 1:
         printf("%s\n\n", slots[index]);
         break;
   }

   return index;
}

static int do_set_success(void *p)
{
   int ret = libabctl_SetBootSuccess();
   return ret;
}

static int do_set_active(void *p)
{
   char *opt = (char *)p;
   unsigned int slot = atoi(opt);

   int ret = libabctl_setActive(slot);

   return ret;
}

static int do_set_unbootable(void *p)
{
   char *opt = (char *)p;
   unsigned int slot = atoi(opt);

   int ret = libabctl_setUnbootable(slot);

   return ret;
}

static int do_debug_prints(void *p)
{
   char *opt = (char *)p;
   unsigned int level = atoi(opt);

   libabctl_debugPrints(level);

   return 0;
}

opt_func_map opt_func_list[] = {
   {"--help",           (sizeof("--help") - 1), &print_help,       0},
   {"--version",        (sizeof("--version") - 1), &print_version,    0},
   {"--set_active",     (sizeof("--set_active") - 1), &do_set_active,    1},
   {"--set_success",    (sizeof("--set_success") - 1), &do_set_success,   0},
   {"--boot_slot",      (sizeof("--boot_slot") - 1), &do_get_boot_slot, 0},
   {"--set_unbootable", (sizeof("--set_unbootable") - 1), &do_set_unbootable, 1},
   {"--dbg",            (sizeof("--dbg") - 1), &do_debug_prints, 1},
};

int main(int argc, char *argv[])
{
   unsigned int i;

   setgid(GID_DISK);
   setuid(UID_SYSTEM);

   if((argc == 1) || (argc >= 4)) {
      usage();
      return 0;
   }

   for(i = 0; i < (sizeof(opt_func_list)/sizeof(opt_func_map)); i++) {
      if (!(strncmp(argv[1], opt_func_list[i].option_string,
                             opt_func_list[i].opt_str_len))) {
         if (opt_func_list[i].is_param_needed == 1) {
            if (argv[2]) {
               opt_func_list[i].action_func(argv[2]);
            } else {
               usage();
            }
         } else {
            opt_func_list[i].action_func(NULL);
         }
         return 0;
      }
   }

   usage();
   return 0;
}
