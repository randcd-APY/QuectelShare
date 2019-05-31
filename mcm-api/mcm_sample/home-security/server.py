#*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
#   Copyright (c) 2013 Qualcomm Technologies, Inc.
#   All rights reserved.
#   Confidential and Proprietary - Qualcomm Technologies, Inc.
#*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

import socket

serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serversocket.bind(('rglasser-linux', 3491))
serversocket.listen(5)

while True:
	conn, add = serversocket.accept()
	buf = conn.recv(64)
	if len(buf) > 0:
		print buf