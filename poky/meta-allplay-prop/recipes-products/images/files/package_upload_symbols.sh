#!/bin/bash

set -e

usage() {
	echo "$(basename $0) [-h|--help] [-u|--upload] --staging <dir> --bindir <dir> --version <version>"
}

# bindir
#   = ${DEPLOY_DIR_IMAGE}
#   = "./tmp-glibc/deploy/images/apq8017-qsap"
# stage
#   = ${SDK_OUTPUT}${SDKTARGETSYSROOT}
#   = "./tmp-glibc/work/apq8017-oe-linux/machine-qsap-image/1.0-r0/sdk/image/usr/local/oecore-x86_64/sysroots/aarch64-oe-linux"

upload=0
while [ $# -gt 0 ]; do
	case "$1" in
		-u|--upload)
			upload=1
			;;
		--staging)
			stage="$2"; shift
			;;
		--bindir)
			bindir="$2"; shift
			;;
		--version)
			version="$2"; shift
			;;
		-h|--help)
			usage
			exit 0
			;;
		*)
			echo "Unknown option '$1'"
			usage
			exit 1
			;;
	esac
	shift
done

if [ -z "$stage" ]; then
	echo "ERROR: No path provided for the staging rootfs."
	usage
	exit 1
fi
if [ -z "$bindir" ]; then
	echo "ERROR: No path provided for the bin directory."
	usage
	exit 1
fi

version=${version// /_}
if [ -z "$version" ]; then
	echo "ERROR: No version provided"
	exit 1
fi

resolveLink() {
	local l="$1"
	local newl
	local maxDepth=10
	for ((i=0; i<maxDepth; i++)); do
		newl=$(readlink "$stage/$l" || :)
		if [ -z "$newl" ]; then
			echo "$l"
			return
		fi
		if [ "x$newl" != "x${newl#/}" ]; then
			# absolute link
			l="$newl"
		else
			# relative link
			l="$(dirname "$l")/$newl"
		fi
	done
	echo "Recursion too deep for $1" >&2
	return 1
}

sym_root="$bindir/symbols"
sym_dir="$sym_root/symbols_hardlink_cache"
ver_dir="$sym_root/$version"

#cp="cp -p"
cp="ln -fn"

echo "building symbols files"
find $stage \( -name ".debug" -prune \) -o \( -type f -printf "%P\\n" \) \
	| while read filename; do

	build_id=$({ readelf -n "$stage/$filename" 2>/dev/null || echo "Build ID: bad"; } | grep "Build ID:" | head -n 1 | grep -o "[0-9a-f]*$" || :)
	[ "$build_id" != "bad" ] || continue

	path_to_file=$(dirname $filename)
	relpath_to_sym_root=../$(echo $path_to_file | sed -re 's;[^/]+;..;g')

	debug_filename="$path_to_file/.debug/$(basename "$filename")"

	if file "$stage/$filename" | grep -q "not stripped"; then
		# app with embedded symbols

		if [ -z "$build_id" ]; then
			# no build id, copy app directly in the version directory
			mkdir -p "$ver_dir/$path_to_file"
			$cp "$stage/$filename" "$ver_dir/$filename"
		else
			# copy app to symbols directory
			if [ ! -e "$sym_dir/$path_to_file/$build_id" ]; then
				mkdir -p "$sym_dir/$path_to_file"
				$cp "$stage/$filename" "$sym_dir/$path_to_file/$build_id"
			fi

			# create link in version directory
			mkdir -p "$ver_dir/$path_to_file"
			ln -fn "$sym_dir/$path_to_file/$build_id" "$ver_dir/$filename"
		fi
	elif file "$stage/$debug_filename" | grep -q "not stripped"; then
		# symbols for stripped app with .debug file

		if [ -z "$build_id" ]; then
			# no build id, copy app and .debug directly in the version directory
			mkdir -p "$ver_dir/$path_to_file/.debug"
			$cp "$stage/$filename" "$ver_dir/$filename"
			$cp "$stage/$debug_filename" "$ver_dir/$debug_filename"

		else
			# copy app and .debug to symbols directory
			if [ ! -e "$sym_dir/$path_to_file/$build_id" ]; then
				mkdir -p "$sym_dir/$path_to_file/.debug"
				$cp "$stage/$filename" "$sym_dir/$path_to_file/$build_id"
				$cp "$stage/$debug_filename" "$sym_dir/$path_to_file/.debug/$build_id"
			fi

			# create links in version directory
			mkdir -p "$ver_dir/$path_to_file/.debug"
			ln -fn "$sym_dir/$path_to_file/$build_id" "$ver_dir/$filename"
			ln -fn "$sym_dir/$path_to_file/.debug/$build_id" "$ver_dir/$debug_filename"
		fi
	fi
done

echo "add symlinks"
find "$stage" -type l -printf "%P\\n" \
	| while read filename; do
		# don't copy the link if the target doesn't have symbols
		link=$(resolveLink "$filename")
		[ -e "$ver_dir/$link" ] || continue
		ln -sfn "$(readlink "$stage/$filename")" "$ver_dir/$filename"
done

echo "archive symbols"
symbol_archive="$bindir/symbols_${version}.tar.gz"
TAR_COMPRESS="-z"
[ -x "/usr/bin/pigz" ] && TAR_COMPRESS="-I /usr/bin/pigz" || TAR_COMPRESS="-z"
# - $sym_dir must be before $ver_dir in the archive so that the latter
# contains the hardlink
# - transform to remove the <$bindir> prefix. Tar already removes the
# leading /, so we need to remove it from $bindir. And "/*" is to remove
# the next "/" if $bindir doesn't end with one
tar $TAR_COMPRESS \
	-cf "${symbol_archive}" \
	--transform "s|${bindir#/}/*||g" \
	"$sym_dir" "$ver_dir"

if [ "$upload" -eq 1 ]; then
	echo "upload symbols"
	CURL=curl
	[ -x /usr/bin/curl ] && CURL=/usr/bin/curl
	$CURL -NsS \
		-F "version=$version" \
		-F "symbol_archive=@${symbol_archive};filename=$(basename $symbol_archive)" \
		--connect-timeout 60 \
		https://apcrs1.qualcomm.com/cgi-bin/submit_symbols.py
fi

rm -rf "${sym_root}"
