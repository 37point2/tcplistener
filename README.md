#Server

Listens on a given port and supports up to 8 concurrent connections via a fork model.
Writes output to log.txt in the current directory.

##Install (optional)
```sh
make
```

##Usage
```sh
./listener $port
```

#Audit

Parses the log output and prints it to stdout.

##Usage
```sh
python audit.py $logfile
```

#Client

Test client for generating traffic.

##Usage
```sh
go run client.go $host $port
```
