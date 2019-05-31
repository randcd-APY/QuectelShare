# Automake script for QMI service IDL files used by QBI

noinst_LTLIBRARIES = libmbim_qmi_svc.la

libmbim_qmi_svc_la_CFLAGS = -Wall -Werror -fno-short-enums
libmbim_qmi_svc_la_CPPFLAGS = -Iapi $(QMIF_CFLAGS)
libmbim_qmi_svc_la_LDFLAGS = -static
libmbim_qmi_svc_la_SOURCES = \
   src/card_application_toolkit_v02.c \
   src/common_v01.c \
   src/control_service_v01.c \
   src/data_common_v01.c \
   src/data_port_mapper_v01.c \
   src/device_management_service_v01.c \
   src/network_access_service_common_v01.c \
   src/network_access_service_v01.c \
   src/phonebook_manager_service_v01.c \
   src/subsystem_control_v01.c \
   src/user_identity_module_v01.c \
   src/voice_service_v02.c \
   src/wireless_data_administrative_service_v01.c \
   src/wireless_data_service_v01.c \
   src/wireless_data_service_common_v01.c \
   src/wireless_messaging_service_v01.c \
   src/persistent_device_configuration_v01.c \
   src/data_system_determination_v01.c \
   src/specific_absorption_rate_v01.c
