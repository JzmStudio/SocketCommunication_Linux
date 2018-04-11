# SocketCommunication_Linux
Communication on web and used on Linux

# Compile
You can compile with two command below:

gcc server.c linkserver.c users.c sql.c userinfor.c -o server -lmysqlclient

gcc client.c link.c -o client -lpthread
