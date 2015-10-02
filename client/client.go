package main

import "runtime"
import "net"
import "os"
import "fmt"
import "time"
import "sync"
import "math/rand"

const letters = "abcdefghijklmnopqrstuvwxyz"

func RandString(min int, max int) string {
	n := rand.Intn(max-min) + min
	b := make([]byte, n)
	for i := range b {
		b[i] = letters[rand.Intn(len(letters))]
	}
	return string(b)
}

func connect(wg *sync.WaitGroup, endpoint string) {
	defer wg.Done()
	conn, err := net.Dial("tcp", endpoint)
	buf := make([]byte, 512)
	defer conn.Close()
	if err == nil {
		fmt.Fprintf(conn, "IDENT\t%s\n", RandString(6,24))
		nr, _ := conn.Read(buf)
		if err == nil {
			fmt.Printf("Received %s\n", string(buf[0:nr]))
			fmt.Fprintf(conn, "AUTH\t%s\n", RandString(6,32))
			nr, _ = conn.Read(buf)
			if err == nil {
				fmt.Printf("Received %s\n", string(buf[0:nr]))
				fmt.Fprintf(conn, "DATA\t%s\n", RandString(10,2048))
				nr, _ = conn.Read(buf)
				if err == nil {
					fmt.Printf("Received %s\n", string(buf[0:nr]))
				}
			}
		}
	}
}

func main() {
	if len(os.Args) < 3 {
		fmt.Printf("Usage: go run client.go $host $port")
		os.Exit(1)
	}
	host := os.Args[1]
	port := os.Args[2]
	endpoint := host + ":" + port
	runtime.GOMAXPROCS(runtime.NumCPU())
	NUM_CLIENTS := 100
	var wg sync.WaitGroup
	wg.Add(NUM_CLIENTS)
	start := time.Now().UnixNano()
	for i:=0; i < NUM_CLIENTS; i++ {
		go connect(&wg, endpoint)
	}
	wg.Wait()
	stop := time.Now().UnixNano()
	fmt.Printf("Took %d milliseconds for %d connections to complete\n", (stop - start)/1000000, NUM_CLIENTS)
}