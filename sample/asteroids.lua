local state = render.state()
local dt = { absolute = 0, accumulator = 0, delta = 0, frame = 0, now = 0,
	step = 1 / 20.0}

local ship = {}
ship.direction = vec3.vec3()
ship.speed = vec3.vec3()
ship.rotate = 0
ship.gas = 0
ship.matrix = { new = mat4.identity(), old = mat4.identity() }
ship.size = 1

local asteroid = {}
local asteroid_spawn = 0
local bullet = {}
local last_fire = 0
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

local fquad = render.new_uint16_t(8)
render.uint16_t_setitem(fquad, 0, -0.5)
render.uint16_t_setitem(fquad, 1, -0.5)
render.uint16_t_setitem(fquad, 2, 0.5)
render.uint16_t_setitem(fquad, 3, -0.5)
render.uint16_t_setitem(fquad, 4, -0.5)
render.uint16_t_setitem(fquad, 5, 0.5)
render.uint16_t_setitem(fquad, 6, 0.5)
render.uint16_t_setitem(fquad, 7, 0.5)

local function new_asteroid(position, direction, size)
	return { position = position, direction = direction, size = size }
end

local function new_bullet(x, y, direction)
	local p = vec3.vec3()
	p.x = x
	p.y = y
	return { position = p, direction = direction }
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
				ship.rotate = 16
			elseif e.key.sym == event.KEY_RIGHT then
				ship.rotate = -16
			end
			-- Gas
			if e.key.sym == event.KEY_UP then
				ship.gas = 0.4
			end
			-- Shoot
			if e.key.sym == event.KEY_SPACE then --and dt.now - last_fire > 0.2 then
				last_fire = dt.now
				local t = vec3.vec3()
				t.y = mat4.get(ship.matrix.new, 0, 0)
				t.x = -mat4.get(ship.matrix.new, 0, 1)
				table.insert(bullet,
					new_bullet(mat4.get(ship.matrix.new, 3, 0),
					mat4.get(ship.matrix.new, 3, 1),
					vec3.mul(t, 18)))
			end
		elseif e.type == event.KEYUP then
			if e.key.sym == event.KEY_LEFT or e.key.sym == event.KEY_RIGHT then
				ship.rotate = 0
			elseif e.key.sym == event.KEY_UP then
				ship.gas = 0
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
	
	render.clear(0, 0, 0)
	
	render.color(1, 1, 1)
	render.load_matrix(mat4.lerp(ship.matrix.old, ship.matrix.new, t).m)
	render_ship()
	--render.render_quad(10)
	--render.render_vertices(fquad, 4)
	render.load_identity()

	for i,b in pairs(bullet) do
		render.push_matrix()
		render.translate(vec3.lerp(vec3.sub(b.position, b.direction),
			b.position, t))
		render.render_quad(2)
		render.pop_matrix()
	end

	for i,b in pairs(asteroid) do
		render.push_matrix()
		render.translate(vec3.lerp(vec3.sub(b.position, b.direction),
			b.position, t))
		render.render_quad(b.size)
		render.pop_matrix()
	end
end

local function ship_update2()
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
		ship.matrix.old = ship.matrix.new
	end
end

render.setup_orthogonal_view(320, 240)
local a = new_asteroid(vec3.vec3(), vec3.vec3(), 20)
a.direction.x = -1
table.insert(asteroid, a)

while true do
	dt.now = event.time()
	dt.delta = dt.now - dt.absolute
	if dt.delta > 0.01 then
		dt.absolute = dt.now
		dt.accumulator = dt.accumulator + dt.delta

		while dt.accumulator >= dt.step do
			if not event_update() then
				mat4.print(ship.matrix.new)
				return
			end

			ship_update2()
			for j,b in pairs(bullet) do
				bullet_update(b)
				if false == on_screen(b.position.x, b.position.y) then
					table.remove(bullet, j)
				end
			end

			for i,a in pairs(asteroid) do
				local b, s, v
				asteroid_update(a)
				b, a.position.x, a.position.y = on_screen(a.position.x,
					a.position.y)

				s, v = circles_collide(mat4.get(ship.matrix.new, 3, 0),
					mat4.get(ship.matrix.new, 3, 1), ship.size, a.position.x,
					a.position.y, a.size)
				if s < 0 then
					ship.matrix.new = mat4.translate(ship.matrix.new, v.x, v.y,
					0)
				end
			end

			-- Collision
			for j,b in pairs(bullet) do
				for i,a in pairs(asteroid) do
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

			dt.accumulator = dt.accumulator - dt.step
			dt.frame = dt.frame + 1
		end
	end

	scene_render()
	render.present()
	event.sleep(1)
end

