#!/bin/sh

AVS_CONF="/etc/alexa/AlexaClientSDKConfig.json"

isAvsConfigured() {
	[ -s "$AVS_CONF" ] || return 1
	jq -e '.authDelegate | has("clientId")' "$AVS_CONF" >/dev/null
}
