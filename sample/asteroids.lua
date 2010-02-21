local client = {}

local state = render.state()
local dt = { absolute = 0, accumulator = 0, delta = 0, frame = 0, now = 0,
	step = 1000 / 20 }

local ship = {} 
local asteroid = {}
local asteroid_spawn = 0
local bullet = {}
local quad = {
		-0.5, -0.5,
		0.5,  -0.5,
		-0.5,   0.5,
		0.5,  0.5}
local triangle = {
	-0.5, -0.5,
	0.0, 0.7,
	0.5, -0.5,
	0.0, -0.3,
}

local server = 0
local socket = 0
local last_send = 0

local function mat4_to_string(m)
	local s = "mat("

	for y=0,3 do
		for x=0,3 do
			s = s .. string.format("%.3f", mat4.get(m, y, x)) .. ","
		end
	end

	return string.sub(s, 1, string.len(s)-1) .. ")"
end

local function string_to_mat4(s)
	local m = mat4.mat4()
	local tmp = {}

	for float in string.gmatch(s, "%-?%d*%.?%d+") do
		table.insert(tmp, tonumber(float))
	end

	for y=0,3 do
		for x=0,3 do
			mat4.set(m, y, x, tmp[y*4 + (x+1)])
		end
	end

	return m
end

local function new_asteroid(position, direction, size)
	return { position = position, direction = direction, size = size }
end

local function new_bullet(x, y, direction)
	local p = vec3.vec3()
	p.x = x
	p.y = y
	return { position = p, direction = direction }
end

local function new_ship()
	return { direction = vec3.vec3(), speed = vec3.vec3(), rotate = 0, gas = 0,
	matrix = { new = mat4.identity(), old = mat4.identity(),
	next = mat4.identity() }, size = 1 }
end

local function on_screen(x, y)
	if x > 165 then
		return false, -165, y
	elseif x < -165 then
		return false, 165, y
	end
	if y > 125 then
		return false, x, -125
	elseif y < -125 then
		return false, x, 125
	end
	return true, x, y
end

local function asteroid_update(b)
	b.position = vec3.add(b.position, b.direction)
end

local bullet_update = asteroid_update

local function circles_collide(x1, y1, r1, x2, y2, r2)
	local v, u, w, r, d, s

	v = vec3.vec3(); v.x = x1; v.y = y1
	u = vec3.vec3(); u.x = x2; u.y = y2
	w = vec3.sub(v, u)
	r = r1 + r2
	d = vec3.length(w)
	w = vec3.normalize(w)
	s = d - r

	return s, vec3.mul(w, -s)
end

local function net_send(msg)
	if client.ip then
		net.send(socket, msg, client.ip, client.port)
	end
end

local function fire_ship(sh)
	local t = vec3.vec3()
	t.y = mat4.get(sh.matrix.new, 0, 0)
	t.x = -mat4.get(sh.matrix.new, 0, 1)
	table.insert(bullet, new_bullet(mat4.get(sh.matrix.new, 3, 0),
		mat4.get(sh.matrix.new, 3, 1),
		vec3.mul(t, 18)))
end

local function event_update()
	local e = event.poll()
	while e.type ~= event.EMPTY do
		if e.type == event.KEYDOWN then
			-- Quit
			if e.key.sym == event.KEY_ESCAPE then
				return false
			end
			-- Rotation
			if e.key.sym == event.KEY_LEFT then
				ship[1].rotate = 16
				net_send("rot:" .. 16)
			elseif e.key.sym == event.KEY_RIGHT then
				ship[1].rotate = -16
				net_send("rot:" .. -16)
			end
			-- Gas
			if e.key.sym == event.KEY_UP then
				ship[1].gas = 0.4
				net_send("gas:" .. 0.4)
			end
			-- Shoot
			if e.key.sym == event.KEY_SPACE then --and dt.now - last_fire > 0.2 then
				fire_ship(ship[1])
				net_send("fire")
			end
		elseif e.type == event.KEYUP then
			if e.key.sym == event.KEY_LEFT or e.key.sym == event.KEY_RIGHT then
				ship[1].rotate = 0
				net_send("rot:" .. 0)
			elseif e.key.sym == event.KEY_UP then
				ship[1].gas = 0
				net_send("gas:" .. 0)
			end
		end
		e = event.poll()
	end
	return true
end

local function render_ship()
	render.scale(10, 10, 10)
	render.render_line(triangle[1], triangle[2], 0, triangle[3], triangle[4], 0)
	render.render_line(triangle[3], triangle[4], 0, triangle[5], triangle[6], 0)
	render.render_line(triangle[5], triangle[6], 0, triangle[7], triangle[8], 0)
	render.render_line(triangle[7], triangle[8], 0, triangle[1], triangle[2], 0)
end

local function scene_render()
	local t = dt.accumulator / dt.step
	if 2 == server then
		t = (dt.now + dt.step - (dt.absolute + dt.step)) / dt.step
	end

	render.clear(0, 0, 0)
	
	for i,s in ipairs(ship) do
		render.color(1, 1, 1)
		render.load_matrix(mat4.lerp(s.matrix.old, s.matrix.new, t).m)
		render_ship()
		--render.render_quad(10)
		--render.render_vertices(fquad, 4)
	end

	render.load_identity()
	for i,b in ipairs(bullet) do
		render.push_matrix()
		render.translate(vec3.lerp(vec3.sub(b.position, b.direction),
			b.position, t))
		render.render_quad(2)
		render.pop_matrix()
	end

	for i,b in ipairs(asteroid) do
		render.push_matrix()
		render.translate(vec3.lerp(vec3.sub(b.position, b.direction),
			b.position, t))
		render.render_quad(b.size)
		render.pop_matrix()
	end
end

local function ship_update(ship)
	local visible, x, y;
	ship.matrix.old = ship.matrix.new

	ship.matrix.new = mat4.rotate(ship.matrix.new, ship.rotate, 0, 0, -1)
	
	ship.speed.y = mat4.get(ship.matrix.new, 0, 0)
	ship.speed.x = -mat4.get(ship.matrix.new, 0, 1)
	ship.direction = vec3.add(ship.direction, vec3.mul(ship.speed, ship.gas))
	if vec3.length(ship.direction) > 8 then
		ship.direction = vec3.mul(vec3.normalize(ship.direction), 8)
	end

	ship.matrix.new = mat4.translate(ship.matrix.new, ship.direction.x,
		ship.direction.y, ship.direction.z)

	visible, x, y = on_screen(mat4.get(ship.matrix.new, 3, 0),
		mat4.get(ship.matrix.new, 3, 1))
	if false == visible then
		mat4.set(ship.matrix.new, 3, 0, x)
		mat4.set(ship.matrix.new, 3, 1, y)
	end

	-- correct the old matrix if we wrapped
	mat4.set(ship.matrix.old, 3, 0, mat4.get(ship.matrix.new, 3, 0))
	mat4.set(ship.matrix.old, 3, 1, mat4.get(ship.matrix.new, 3, 1))
	ship.matrix.old = mat4.translate(ship.matrix.old, -ship.direction.x,
	-ship.direction.y, -ship.direction.z)
end


-- select server/client
local function select_connection()
	while 0 == server do
		local e = event.poll()
		if event.KEYDOWN == e.type then
			if event.KEY_1 == e.key.sym then
				server = 1
				socket = net.open_udp(0, 5678)
				print("server: waiting for connections")
			else
				server = 2
				socket = net.open_udp(net.resolve_host("localhost"), 0)
				client.ip = net.iptoi("127.0.0.1")
				client.port = 5678
				net.send(socket, "hello",  client.ip, client.port)
				event.sleep(1000)
				local p = net.recv(socket)
				if "hi" == p.msg then
					print("connected!")
					table.insert(ship, new_ship())
				end
			end
		end
	end
end

local function update_entities()
	for i,s in ipairs(ship) do
		ship_update(s)
	end

	for j,b in ipairs(bullet) do
		bullet_update(b)
		if false == on_screen(b.position.x, b.position.y) then
			table.remove(bullet, j)
		end
	end

	for i,a in ipairs(asteroid) do
		local b, s, v
		asteroid_update(a)
		b, a.position.x, a.position.y = on_screen(a.position.x,
			a.position.y)

		for j,sh in ipairs(ship) do
			s, v = circles_collide(mat4.get(sh.matrix.new, 3, 0),
				mat4.get(sh.matrix.new, 3, 1), sh.size, a.position.x,
				a.position.y, a.size)
			if s < 0 then
				sh.matrix.new = mat4.translate(sh.matrix.new, v.x, v.y,
				0)
			end
		end
	end

	-- Collision
	for j,b in ipairs(bullet) do
		for i,a in ipairs(asteroid) do
			local p

			p, _ = circles_collide(b.position.x, b.position.y, 1,
			a.position.x, a.position.y, a.size)
			if p <= 0 then
				table.remove(bullet, j)
				table.remove(asteroid, i)

				if a.size >= 10 then
					local tmp
					tmp = new_asteroid(vec3.vec3(), vec3.vec3(), a.size/2)
					tmp.direction.x = a.direction.x / 2
					tmp.direction.y = a.direction.x / 2
					tmp.position = a.position
					table.insert(asteroid, tmp)
					tmp = new_asteroid(vec3.vec3(), vec3.vec3(), a.size/2)
					tmp.direction.x = a.direction.x / 2
					tmp.direction.y = -a.direction.x / 2
					tmp.position = a.position
					table.insert(asteroid, tmp)
				end
			end
		end
	end
end

local function update_network(time)
	local p = net.recv(socket)
	local recv = false

	while "" ~= p.msg do
		if "hello" == p.msg then
			client.ip = p.ip
			client.port = p.port
			net.send(socket, "hi", p.ip, p.port)
			table.insert(ship, new_ship())
		elseif "quit" == p.msg then
		else
			_, _, k, v = string.find(p.msg, "(%a+):(.+)")
			if "rot" == k then ship[2].rotate = tonumber(v)
			elseif "gas" == k then ship[2].gas = tonumber(v)
			elseif "fire" == p.msg then fire_ship(ship[2])
			elseif "ship" == k then
				_, _, id, mat = string.find(v, "(%d+):(.+)")
				id = tonumber(id)
				ship[id].matrix.old = ship[id].matrix.new
				ship[id].matrix.new = ship[id].matrix.next
				ship[id].matrix.next = string_to_mat4(mat)
			else
				print("unknown msg: " .. p.msg)
			end
		end

		p = net.recv(socket)
		recv = true
	end

	return recv
end

-- main loop
local function update_game()
	while true do
		dt.now = event.time()
		dt.delta = dt.now - dt.absolute
		if dt.delta > 1 and (1 == server or (2 == server and true ==
			update_network())) then
			dt.absolute = dt.now
			dt.accumulator = dt.accumulator + dt.delta

			while dt.accumulator >= dt.step do
				if not event_update() then
					return
				end

				if 1 == server then
					update_network()
					update_entities()
					if client.ip then
						net_send("ship:1:" .. mat4_to_string(
							ship[2].matrix.new))
						net_send("ship:2:" .. mat4_to_string(
							ship[1].matrix.new))
						last_send = dt.now
					end
				end


				dt.accumulator = dt.accumulator - dt.step
				dt.frame = dt.frame + 1
			end
		end

		scene_render()
		render.present()
		event.sleep(1)
	end
end


render.create_window(640, 480)
table.insert(ship, new_ship())
select_connection()
render.setup_orthogonal_view(320, 240)
local a = new_asteroid(vec3.vec3(), vec3.vec3(), 20)
a.direction.x = -1
table.insert(asteroid, a)

update_game()
--net.send(socket, "quit" ...
render.quit()

