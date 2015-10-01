package main

import "runtime"
import "net"
import "fmt"
import "time"
import "sync"
import "math/rand"

const letters = "abcdefghijklmnopqrstuvwxyz"

func RandString(n int) string {
	b := make([]byte, n)
	for i := range b {
		b[i] = letters[rand.Intn(len(letters))]
	}
	return string(b)
}

func connect(wg *sync.WaitGroup) {
	defer wg.Done()
	conn, err := net.Dial("tcp", "127.0.0.1:5555")
	buf := make([]byte, 512)
	defer conn.Close()
	if err == nil {
		fmt.Fprintf(conn, "IDENT\t%s\n", RandString(rand.Intn(24)))
		nr, _ := conn.Read(buf)
		if err == nil {
			fmt.Printf("Received %s\n", string(buf[0:nr]))
			fmt.Fprintf(conn, "AUTH\t%s\n", RandString(rand.Intn(32)))
			nr, _ = conn.Read(buf)
			if err == nil {
				fmt.Printf("Received %s\n", string(buf[0:nr]))
				fmt.Fprintf(conn, "DATA\t%s\n", RandString(rand.Intn(2048)))
				nr, _ = conn.Read(buf)
				if err == nil {
					fmt.Printf("Received %s\n", string(buf[0:nr]))
				}
			}
		}
	}
}

func main() {
	runtime.GOMAXPROCS(runtime.NumCPU())
	NUM_CLIENTS := 100
	var wg sync.WaitGroup
	wg.Add(NUM_CLIENTS)
	start := time.Now().UnixNano()
	for i:=0; i < NUM_CLIENTS; i++ {
		go connect(&wg)
	}
	wg.Wait()
	stop := time.Now().UnixNano()
	fmt.Printf("Took %d milliseconds for %d connections to complete\n", (stop - start)/1000000, NUM_CLIENTS)
}