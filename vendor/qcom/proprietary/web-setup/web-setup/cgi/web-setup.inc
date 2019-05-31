# Copyright (C) 2017 Qualcomm Technologies, Inc.
# All rights reserved
# Confidential and Proprietary - Qualcomm Technologies, Inc.

IFS= read -r -n 2048 cgiParams

# get the value of a specfic CGI variable
# usage: myVar="$(cgiVar my_var)" || error
cgiVar() {
	local varName="$1"

	local fields="$cgiParams"
	while [ -n "$fields" ]; do
		field="$(printf "%s" "$fields" | cut -d "&" -f 1)"
		fields="$(printf "%s" "$fields" | cut -s -d "&" -f 2-)"

		name="$(printf "%s" "$field" | cut -d = -f 1)"
		name="$(printf "%b" "$(printf "%s" "$name" | sed -e 's/\+/ /g' -e 's/\\/%5C/g' -re 's/%([0-9a-f]{2})/\\x\1/gI')")"

		if [ "$name" == "$varName" ]; then
			val="$(printf "%s" "$field" | cut -s -d = -f 2-)"
			val="$(printf "%b" "$(printf "%s" "$val" | sed -e 's/\+/ /g' -e 's/\\/%5C/g' -re 's/%([0-9a-f]{2})/\\x\1/gI')")"

			printf "%s" "$val"
			return 0
		fi
	done
	return 1
}

output_html_error() {
	# error must be of the form "<code> <description>", e.g. "404 Not Found"
	local error="$1"
	cat <<-EOF
		Status: $error
		Content-Type: text/html

		<html>
			<head>
				<title>$error</title>
			</head>
			<body>
				<h1>$error</h1>
			</body>
		</html>
	EOF

	exit 0
}


output_json() {
	echo 'Content-Type: application/json'
	echo
	cat
}

json_escape() {
	if [ $# -gt 0 ]; then
		printf "%s" "$*" | jq '.' -Rs
	else
		jq '.' -Rs
	fi
}

