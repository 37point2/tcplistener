import sys

class Client():
	sessionid = 0
	ident = ""
	auth = ""

def main():
	if len(sys.argv) < 2:
		print("Usage: python audit.py $logfile")
		sys.exit(1)
	clients = {}
	with open(sys.argv[1], "r") as fd:
		while True:
			line = fd.readline()
			line = line.rstrip()
			if not line: break
			try:
				sessionid, messagetype, data = line.split("\t")
				if messagetype == "IDENT":
					client = Client()
					client.sessionid = sessionid
					client.ident = data
					clients[sessionid] = client
				elif messagetype == "AUTH":
					clients[sessionid].auth = data
				elif messagetype == "DATA":
					client = clients[sessionid]
					print(str.format("Sessionid - {}\nIDENT - {}\nAUTH - {}\nDATA - {}\n", client.sessionid, client.ident, client.auth, data))
					clients.pop(sessionid, None)
			except Exception as e:
				print(e)
				print(line)
				break

if __name__ == "__main__":
	main()