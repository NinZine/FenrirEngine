local p = string.split(package.path, "[^%?]+")
package.path = "LD19/?.lua;" .. p[1] .. package.path

require 'hud'

local state = render.create_window(480, 320)
render.bind_buffers(state)
state.depth = render.generate_depthbuffer(state.width, state.height) -- Fail
render.set_viewport(0, 0, 480, 320)
local distance = 2.5 
local zoom = 0
local light = vec3.vec3()
light.x, light.y, light.z = 0, 10, 2.5 

--[[
render.enable_light(0)
render.setup_ambient_light(0, 0.2, 0.2, 0.5)
render.setup_diffuse_light(0, 0.2, 0.2, 0.2)
render.set_light_position(0, light)]]--

local game = {camera = { yaw = 0, pitch = 0}, mouse = {x = 0, y = 0}}

local function shaderLight()
	local s = shader.create("tests/lighting.vert", "tests/lighting.frag")

	local	lightPosition,
			directionalLight,
			lightColor,
			ambient,
			shininess,
			materialDiffuse,
			materialSpecular
			=
			vec3.vec3(),
			vec3.vec3(),
			quat.quat(),
			quat.quat(),
			178.0,
			vec3.vec3(),
			vec3.vec3()

	lightPosition.x, lightPosition.y, lightPosition.z = -2.0, 1.0, 20.0
	directionalLight.x,  directionalLight.y, directionalLight.z
		= -1.0, 1.0, 12.0
	lightColor.x, lightColor.y,lightColor.z = 0.80, 0.80, 0.62, 1.0
	ambient.x, ambient.y, ambient.z = 0.13, 0.13, 0.12, 1.0
	materialDiffuse.x, materialDiffuse.y, materialDiffuse.z = 0.89, 0.63, 0.33
	materialSpecular.x, materialSpecular.y, materialSpecular.z
		= 0.93, 0.93, 0.93
	
	shader.use(s)
	shader.set_uniform3f(s, "lightPosition", lightPosition)
	--shader.set_uniform3f(s, "directionalLight", directionalLight)
	shader.set_uniform4f(s, "lightColor", lightColor)
	shader.set_uniform4f(s, "ambient", ambient)
	shader.set_uniform1f(s, "shininess", shininess)
	shader.set_uniform3f(s, "materialDiffuse", materialDiffuse)
	shader.set_uniform3f(s, "materialSpecular", materialSpecular)

	return s
end

local function gourardShader()
	local s = shader.create("tests/gourard.vert", "")
	--render.enable_light(0)
	render.setup_ambient_light(0, 0.2, 0.2, 0.5)
	render.setup_diffuse_light(0, 0.2, 0.2, 0.2)
	render.set_light_position(0, light)
	
	shader.use(s)
end

game.camera.position = vec3.vec3()
game.camera.position.z = -50
game.camera.lookAt = vec3.vec3()
game.camera.speed = vec3.vec3()

hud.set_font(font.load("LD19/04b_03.ttf", 8))

event.show_cursor(0)
local lightShader = shaderLight()
--gourardShader()

function game.setup_rendering()
	-- Setup camera and rendering
    render.bind_depthbuffer(state)
    render.setup_perspective_view(60, 480/320, 0.01, 500.0)
	render.enable_depth()
	render.enable_culling(2)
    render.clear(0.0, 0.0, 0.0)
	render.load_identity()
	render.color(1.0, 1.0, 1.0)
	render.look_at(
		game.camera.position.x, game.camera.position.y, game.camera.position.z,
		game.camera.position.x + game.camera.lookAt.x,
		game.camera.position.y + game.camera.lookAt.y,
		game.camera.position.z + game.camera.lookAt.z,
		0, 1, 0)
	--[[render.look_at(0, 0, 50,
		0, 0, 0,
		0, 1, 0)]]--
	--render.rotate(-90, 1, 0, 0)
end

function game.update_camera(camera)
	camera.position = vec3.add(camera.position, camera.speed)
end

function game.update_camera_lookAt(x, y)
	local mouseSpeed = 0.001

	game.camera.yaw = game.camera.yaw - x * mouseSpeed
	game.camera.pitch = game.camera.pitch - y * mouseSpeed

	if game.camera.yaw > math.pi then
		game.camera.yaw = game.camera.yaw - math.pi * 2
	elseif game.camera.yaw < -math.pi then
		game.camera.yaw = game.camera.yaw + math.pi * 2
	end

	if game.camera.pitch > math.pi / 2 then
		game.camera.pitch = math.pi / 2
	elseif game.camera.pitch < -math.pi / 2 then
		game.camera.pitch = -math.pi / 2
	end

	game.camera.lookAt.x =
		math.sin(game.camera.yaw) * math.cos(game.camera.pitch)
	game.camera.lookAt.y = math.sin(game.camera.pitch)
	game.camera.lookAt.z =
		math.cos(game.camera.yaw) * math.cos(game.camera.pitch)
end

function update()
	local e = event.poll()
	while e.type ~= event.EMPTY do
        if e.type == event.KEYDOWN then
			if e.key.sym == event.KEY_BACKQUOTE then
				console.toggle()
			elseif console.showing == true then
				console.key_down(e.key)
			else
				if e.key.sym == event.KEY_ESCAPE then
					return false
				elseif e.key.sym == event.KEY_DOWN then
					game.camera.speed.x = -game.camera.lookAt.x
					game.camera.speed.z = -game.camera.lookAt.z
				elseif e.key.sym == event.KEY_UP then
					game.camera.speed.x = game.camera.lookAt.x
					game.camera.speed.z = game.camera.lookAt.z
				elseif e.key.sym == event.KEY_LEFT then
					game.camera.speed.x = game.camera.lookAt.z
					game.camera.speed.z = -game.camera.lookAt.x
				elseif e.key.sym == event.KEY_RIGHT then
					game.camera.speed.x = -game.camera.lookAt.z
					game.camera.speed.z = game.camera.lookAt.x
				end
			end
		elseif e.type == event.KEYUP then
			if console.showing == true then
				console.key_up(e.key)
			else
				if e.key.sym == event.KEY_UP
				or e.key.sym == event.KEY_DOWN then
					game.camera.speed.x = 0
					game.camera.speed.z = 0
				elseif e.key.sym == event.KEY_LEFT
				or e.key.sym == event.KEY_RIGHT then
					game.camera.speed.x = 0
					game.camera.speed.z = 0
				end
			end
		elseif e.type == event.TOUCHDOWN then
			game.mouse.x, game.mouse.y =
				e.touch.dx - e.touch.sx,
				e.touch.dy - e.touch.sy
			game.update_camera_lookAt(game.mouse.x, game.mouse.y)
			event.warp_mouse(240, 160)
		end
		e = event.poll()
	end

	game.update_camera(game.camera)
	game.setup_rendering()
	--render.enable_light(0)
	shader.use(lightShader)
	-- Render cubes
	for y=0, 20 do
		for x=0, 20 do
			render.push_matrix()
			render.translate(x, y, 0)
			render.render_cube(1)
			render.pop_matrix()
		end
	end
	shader.use(0)
	--render.disable_light(0)

	hud.draw_text(string.format("(%.3f,%.3f)",
		game.mouse.x,
		game.mouse.y),
		140, -120)
	hud.draw_text(string.format("(%.3f,%.3f,%.3f)",
		game.camera.position.x,
		game.camera.position.y,
		game.camera.position.z),
		140, -140)
	hud.draw_text(string.format("(%.3f,%.3f,%.3f)",
		game.camera.lookAt.x,
		game.camera.lookAt.y,
		game.camera.lookAt.z),
		140, -150)

	if console.showing == true then
		console.render()
	end

	render.present()

	event.sleep(1)
	return true
end
