-- server
server_socket = net.open_udp(0, 45678)
print("server: " .. server_socket)

-- client
client_socket = net.open_udp(net.resolve_host("localhost"), 0)
print("client: " .. client_socket)

net.send(client_socket, "hello world", net.iptoi("127.0.0.1"), 45678)
net.close(client_socket)

event.sleep(2000)

msg = net.recv(server_socket)
print("received: " .. msg)
net.close(server_socket)

