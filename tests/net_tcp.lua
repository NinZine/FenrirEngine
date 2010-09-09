-- server
--server_socket = net.open_udp(0, 45678)
--print("server: " .. server_socket)

-- client
ip = net.resolve_host("www.google.com")
client_socket = net.open_tcp(ip, 80)
print("client: " .. client_socket)
if client_socket == -1 then return false end

net.send(client_socket, "GET / HTTP/1.0\n\n", 0, 0)
--net.close(client_socket)

event.sleep(2000)
p = net.recv(client_socket)

--p.msg = string.rep(p.msg, 1) -- setting p.msg seems to first free() it.
--q = net.recv(server_socket) -- modifies p.msg. not anymore :p
if "" ~= p.msg then
	print("ip: " .. net.itoip(p.ip))
	print("port: " .. p.port)
	print("msg: " .. p.msg)
else
	return false
end

--[[net.send(server_socket, "hey!", p.ip, p.port)

event.sleep(2000)
q = net.recv(client_socket)
if "" ~= q.msg then
	print("ip: " .. net.itoip(q.ip))
	print("port: " .. q.port)
	print("msg: " .. q.msg)
end


net.close(server_socket)
net.close(client_socket)

return false
]]--
-- is p.msg and q.msg freed here?

return false 

