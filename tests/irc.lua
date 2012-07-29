local p = string.split(package.path, "[^%?]+")
package.path = "LD19/?.lua;" .. p[1] .. package.path

require 'irc'
require 'hud'

local socket = irc.connect("irc.quakenet.org", 6667, {
	user = "ninbot",
	nick = "ninbot"
})
print("irc> socket: " .. socket)

function update()
	if socket == -1 then return false end


	local pkt = net.recv(socket)
	while "" ~= pkt.msg do
		-- Break on CRLF
		for _, msg in ipairs(irc.parse(pkt.msg)) do
			print("irc> " .. msg)
			-- Parse command?
			if string.find(msg, "^PING.+") == 1 then
				local pong = "PONG" .. string.sub(msg, 5, -1)
				irc.send(socket, pong)
			end
		end

		--net.free(pkt)
		pkt = net.recv(socket)
	end

	event.sleep(1)
end
