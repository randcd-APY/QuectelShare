/*
* Copyright (c) 2017, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <wayland-client.h>
#include <wayland-server.h>
#include <wayland-client-protocol.h>

#include "pll-client-protocol.h"

struct wl_display *display = NULL;
struct wl_pll *pll = NULL;

static void global_registry_handler(void *data, struct wl_registry *registry,
	uint32_t id, const char *interface, uint32_t version)
{
	printf("Got a registry event for %s id %d\n", interface, id);
	if (strcmp(interface, "wl_pll") == 0)
		pll = wl_registry_bind(registry,
				id,
				&wl_pll_interface,
				1);
}

static void global_registry_remover(void *data,
	struct wl_registry *registry, uint32_t id)
{
	printf("Got a registry losing event for %d\n", id);
}

static const struct wl_registry_listener registry_listener = {
	global_registry_handler,
	global_registry_remover
};

int main(int argc, char **argv)
{
	int ppms[] = {50000, 50000, -50000};
	int i;
	int rc = 0;

	display = wl_display_connect(NULL);
	if (!display) {
		fprintf(stderr, "failed to create display\n");
		return -1;
	}
	printf("connected to display\n");

	struct wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_listener, NULL);

	wl_display_dispatch(display);
	wl_display_roundtrip(display);

	if (pll == NULL) {
		fprintf(stderr, "Can't find wl_pll\n");
		return -1;
	}

	printf("enable display pll update\n");
	wl_pll_enable_ppm(pll, 1);
	rc = wl_display_flush(display);
	if (rc < 0)
		fprintf(stderr, "failed to flush display\n");

	for (i = 0; i < sizeof(ppms)/sizeof(int); i++) {
		printf("update display pll by %d\n", ppms[i]);
		wl_pll_set_ppm(pll, ppms[i]);

		// Flush requests to server
		rc = wl_display_flush(display);
		if (rc < 0)
			fprintf(stderr, "failed to flush display\n");

		sleep(3);
	}

	printf("disable display pll update\n");
	wl_pll_enable_ppm(pll, 0);
	rc = wl_display_flush(display);
	if (rc < 0)
		fprintf(stderr, "failed to flush display\n");


	wl_display_disconnect(display);
	printf("disconnected from display\n");

	return 0;
}
