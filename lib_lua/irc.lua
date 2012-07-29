local p = {}
irc = p

local function split(str, sub)
	-- Alternative method, but this one is already faster
	--return string.split(str, "[^" .. sub .. "]+")

	local explode = {}

	local sub_len = string.len(sub)
	local i = string.find(str, sub)

	while i ~= nil do
		table.insert(explode, string.sub(str, 1, i-1))
		str = string.sub(str, i+sub_len)
		i = string.find(str, sub)
	end
	return explode
end

function p.connect(hostname, port, conf)
	local ip = net.resolve_host(hostname)
	local socket = net.open_tcp(ip, port)

	if socket == 0 then return 0 end

	irc.send(socket, "NICK " .. conf.nick)
	irc.send(socket, string.format(
		"USER %s 0 * :%s", conf.user, conf.user)
	)
	
	return socket
end

function p.parse(msg)
	return split(msg, "\r\n")
end

function p.send(socket, msg)
	print("irc> " .. msg)
	net.send(socket, msg .. "\r\n", 0, 0)
end

return irc

