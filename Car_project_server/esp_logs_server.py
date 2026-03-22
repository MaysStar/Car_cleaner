import socket

#listen every ip
UDP_IP = "0.0.0.0" 

#default udp port
UDP_PORT = 12345 

# create upd socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#connect ip and port to socket
sock.bind((UDP_IP, UDP_PORT))

#permonent data prosessing
while True:
    try:
        #get till 1Kb in one time
        data, addr = sock.recvfrom(1024)
        
        # decode data and ignore errors 
        log_message = data.decode('utf-8', errors='ignore')
        
        #print data into screen
        print(f"[{addr[0]}] {log_message}", end='')
        
    except KeyboardInterrupt:
        print("\nSERVER STOP")
        break
    except Exception as e:
        print(f"\nERROR CODE:{e}")