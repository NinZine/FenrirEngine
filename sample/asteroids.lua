local state = render.new_state()
local dt = { absolute = 0, accumulator = 0, delta = 0, frame = 0, now = 0,
	step = 1 / 20.0}

local ship = {}
ship.position = vec3.new_vec3()
ship.position.x = 0
ship.position.y = 0
ship.direction = vec3.new_vec3()

ship.speed = vec3.new_vec3() 
ship.rotation = 0
ship.old_rotation = 0
ship.rotate = 0
ship.gas = 0

local asteroid = {}
local bullet = {}
local last_fire = 0

local function new_bullet(position, direction)
	return { position = position, direction = direction }
end

local function on_screen(p)
	if p.position.x > 165 then
		p.position.x = -165
		return false
	elseif p.position.x < -165 then
		p.position.x = 165
		return false
	end
	if p.position.y > 125 then
		p.position.y = -125
		return false
	elseif ship.position.y < -125 then
		p.position.y = 125
		return false
	end
	return true
end

local function asteroid_update(b)
	b.position = vec3.add(b.position, b.direction)
end

local bullet_update = asteroid_update

local function event_update()
	local e = event.poll()
	while e.type ~= event.EMPTY do
		if e.type == event.KEYDOWN then
			-- Quit
			if e.key.c == "q" then
				return false
			end
			-- Rotation
			if e.key.sym == 276 then
				ship.rotate = 16 
			elseif e.key.sym == 275 then
				ship.rotate = -16
			end
			-- Gas
			if e.key.sym == 273 then
				ship.gas = 0.4
			end
			-- Shoot
			if e.key.c == " " then --and dt.now - last_fire > 0.2 then
				last_fire = dt.now
				local t = vec3.new_vec3()
				t.x = -math.sin(math.rad(ship.rotation))
				t.y = math.cos(math.rad(ship.rotation))
				table.insert(bullet, new_bullet(ship.position, vec3.mul(t, 20)))
			end
		elseif e.type == event.KEYUP then
			if e.key.sym == 275 or e.key.sym == 276 then
				ship.rotate = 0
			elseif e.key.sym == 273 then
				ship.gas = 0
			end
		end
		e = event.poll()
	end
	return true
end

local function scene_render()
	local t = dt.accumulator / dt.step
	render.clear(0, 0, 0)
	render.load_identity()

	render.push_matrix()
	render.color(1, 1, 1)
	render.translate(vec3.lerp(vec3.sub(ship.position, ship.direction),
		ship.position, t))
	render.rotate(ship.old_rotation + ((ship.rotation - ship.old_rotation) * t),
		0, 0, 1)
	render.render_quad(10)
	render.pop_matrix()

	for i,b in pairs(bullet) do
		render.push_matrix()
		render.translate(vec3.lerp(vec3.sub(b.position, b.direction),
			b.position, t))
		render.render_quad(2)
		render.pop_matrix()
	end
end

local function ship_update()
	ship.old_rotation = ship.rotation
	ship.rotation = ship.rotation + ship.rotate
	--[[if ship.rotation > 359 then ship.rotation = ship.rotation - 359 
	elseif ship.rotation < 0 then ship.rotation = ship.rotation + 359
	end]]--

	ship.speed.y = math.cos(math.rad(ship.rotation))
	ship.speed.x = -math.sin(math.rad(ship.rotation))
	ship.direction = vec3.add(ship.direction, vec3.mul(ship.speed, ship.gas))
	if vec3.length(ship.direction) > 8 then
		ship.direction = vec3.mul(vec3.normalize(ship.direction), 8)
	end
	ship.position = vec3.add(ship.position, ship.direction)
	if ship.position.x > 165 then
		ship.position.x = -165
	elseif ship.position.x < -165 then
		ship.position.x = 165
	end
	if ship.position.y > 125 then
		ship.position.y = -125
	elseif ship.position.y < -125 then
		ship.position.y = 125
	end

end

render.setup_orthogonal_view(320, 240)
while true do
	dt.now = event.time()
	dt.delta = dt.now - dt.absolute
	if dt.delta > 0.01 then
		dt.absolute = dt.now
		dt.accumulator = dt.accumulator + dt.delta

		while dt.accumulator >= dt.step do
			if not event_update() then
				return
			end

			ship_update()
			for i,b in pairs(bullet) do
				bullet_update(b)
				if false == on_screen(b) then table.remove(bullet, i) end
			end

			dt.accumulator = dt.accumulator - dt.step
			dt.frame = dt.frame + 1
		end
	end

	scene_render()
	render.present()
	event.sleep(1)
end

