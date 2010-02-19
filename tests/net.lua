-- server
server_socket = net.open_udp(0, 45678)
print("server: " .. server_socket)

-- client
client_socket = net.open_udp(net.resolve_host("localhost"), 0)
print("client: " .. client_socket)

net.send(client_socket, "hello world", net.iptoi("127.0.0.1"), 45678)
--net.close(client_socket)

p = net.recv(server_socket)
event.sleep(2000)

p.msg = string.rep(p.msg, 1) -- setting p.msg seems to first free() it.
q = net.recv(server_socket) -- modifies p.msg. not anymore :p
if "" ~= p.msg then
	print("ip: " .. net.itoip(p.ip))
	print("port: " .. p.port)
	print("msg: " .. p.msg)
end

net.send(server_socket, "hey!", p.ip, p.port)

event.sleep(2000)
q = net.recv(client_socket)
if "" ~= q.msg then
	print("ip: " .. net.itoip(q.ip))
	print("port: " .. q.port)
	print("msg: " .. q.msg)
end


net.close(server_socket)
net.close(client_socket)

-- is p.msg and q.msg freed here?

