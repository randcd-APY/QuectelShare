# location open source packages
IMAGE_INSTALL += "${@base_contains('DISTRO_FEATURES','location-services', 'loc-hal', '', d)}"
IMAGE_INSTALL += "${@base_contains('DISTRO_FEATURES','location-services', 'loc-api-v02', '', d)}"
IMAGE_INSTALL += "${@base_contains('DISTRO_FEATURES','location-services', 'loc-ds', '', d)}"
IMAGE_INSTALL += "${@base_contains('DISTRO_FEATURES','location-services', 'loc-net-iface', '', d)}"
