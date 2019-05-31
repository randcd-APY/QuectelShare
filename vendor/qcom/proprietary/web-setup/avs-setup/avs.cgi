#!/bin/sh

# Copyright (C) 2017 Qualcomm Technologies, Inc.
# All rights reserved
# Confidential and Proprietary - Qualcomm Technologies, Inc.

source web-setup.inc
source /usr/share/mdns_avs_functions.sh

AVS_PIPE="/run/avs.fifo"
TMP_AVS_CONF="${TMPDIR:-/tmp}/avs.conf"
DEV_CONF="/persist/device.conf"

avs_get_meta() {
	local codeVerifier
	local codeChallenge
	local productId
	local deviceSn

	[ -f "$DEV_CONF" ] || output_html_error "500 Internal Server Error"

	if ! isAvsConfigured; then
		[ -f "$TMP_AVS_CONF" ] || echo '{}' > "$TMP_AVS_CONF"
		codeVerifier="$(jq -r '.authDelegate.codeVerifier//empty' "$TMP_AVS_CONF")"
		if [ -z "$codeVerifier" ]; then
			codeVerifier="$(openssl rand 32 | openssl base64 | tr -d '=\n' | tr '+/' '-_')"
			jq '.authDelegate |= . + {
					"codeVerifier": '"$(json_escape "$codeVerifier")"',
				} ' "$TMP_AVS_CONF" > "$TMP_AVS_CONF.tmp" \
				&& mv "$TMP_AVS_CONF.tmp" "$TMP_AVS_CONF"
		fi
		codeChallenge="$(printf "%s" "$codeVerifier" | openssl sha256 -binary | openssl base64 | tr -d '=\n' | tr '+/' '-_')"
	fi

	productId="$(jq -r '.productId//empty' "$DEV_CONF")"
	[ -n "$productId" ] || output_html_error "500 Internal Server Error"
	deviceSn="$(jq -r '.serial//empty' "$DEV_CONF")"
	[ -n "$deviceSn" ] || output_html_error "500 Internal Server Error"

	q='"' # hack because the shell removes the " in ${...} otherwise, and \" is printed as-is
	output_json <<-EOF
	{
		${codeChallenge:+${q}codeChallenge${q}: $(json_escape "$codeChallenge"),}
		${codeChallenge:+${q}method${q}: ${q}S256${q},}
		"productId": $(json_escape "$productId"),
		"deviceSn": $(json_escape "$deviceSn")
	}
	EOF
}

avs_set_auth() {
	local avsAuth="$(cgiVar avsAuth)"
	local avsClientId="$(cgiVar avsClientId)"
	local avsRedirectUri="$(cgiVar avsRedirectUri)"

	local response
	local avsConf
	local clientSecret
	local deviceSn
	local productId

	[ "$SERVER_PORT" = "443" ] || output_html_error "403 Forbidden"
	[ -n "$avsAuth" ] && [ -n "$avsClientId" ] && [ -n "$avsRedirectUri" ] || output_html_error "400 Bad Request"

	isAvsConfigured && output_html_error "500 Internal Server Error"

	response="$(curl -Ls \
		--data-raw grant_type=authorization_code \
		--data-raw code="$avsAuth" \
		--data-raw redirect_uri="$avsRedirectUri" \
		--data-raw client_id="$avsClientId" \
		--data-raw code_verifier="$(jq -r '.authDelegate.codeVerifier//empty' "$TMP_AVS_CONF")" \
		https://api.amazon.com/auth/O2/token)" \
		|| output_html_error "500 Internal Server Error"

	# validate that the response is valid json
	printf "%s" "$response" | jq "{}" || output_html_error "500 Internal Server Error"

	if [ -n "$(printf "%s" "$response" | jq -r '.error//empty')" ]; then
		echo -n "LWA error: $response" >&2
		output_html_error "500 Internal Server Error"
	fi

	rm -f "$TMP_AVS_CONF"

	clientSecret="$(jq -r '.avsClientSecret//empty' "$DEV_CONF")"
	[ -n "$clientSecret" ] || output_html_error "500 Internal Server Error"
	deviceSn="$(jq -r '.serial//empty' "$DEV_CONF")"
	[ -n "$deviceSn" ] || output_html_error "500 Internal Server Error"
	productId="$(jq -r '.productId//empty' "$DEV_CONF")"
	[ -n "$productId" ] || output_html_error "500 Internal Server Error"

	[ -p "$AVS_PIPE" ] || output_html_error "500 Internal Server Error"

	printf "%s" "$response" | jq '{ "authDelegate" : {
		"deviceTypeId": '"$(json_escape "$productId")"',
		"deviceSerialNumber": '"$(json_escape "$deviceSn")"',
		"clientId": '"$(json_escape "$avsClientId")"',
		"clientSecret": '"$(json_escape "$clientSecret")"',
		"refreshToken": .refresh_token
	} }' > "$AVS_PIPE"

	output_json <<-EOF
	{}
	EOF
}

case "$PATH_INFO" in
	/get_meta)
		avs_get_meta
		;;
	/set_auth)
		avs_set_auth
		;;
	*)
		output_html_error "404 Not Found"
		;;
esac
