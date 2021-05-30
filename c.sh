rm ./log/cli*.log c 

gcc client/client.c src/cli_accessusername.c src/my_zlog_init.c  src/clienthelp.c src/wrap.c -g -Wall -o c -I./lib/  -lpthread -lzlog

#./c 127.0.0.1 5000 
./c -h
