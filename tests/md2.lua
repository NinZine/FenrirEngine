local state = render.state()
render.create_window(320, 480)
--render.setup_orthogonal_view(800, 600)
render.setup_perspective_view(60, 320/480, 0.01, 500.0)
render.set_viewport(0, 0, 320, 480)
state.depth = render.generate_depthbuffer(320, 480)

local i = -50
local dir = 1
local rotate = 0
local rot = 0
local zoom = 0
local img = image.load("tests/megaman.png")
local tex = render.upload_texture(img.w, img.h, img.bpp, img.type, img.data)
image.free(img)
local b = md2.open("tests/megaman.md2", tex)
--md2.set_animation(b, 0, 39, 9)
md2.set_animation(b, md2.KEMD2AnimationRun)

render.enable_culling(1)
function update()
    e = event.poll()
    while e.type ~= event.EMPTY do
        if e.type == event.KEYDOWN then
            if e.key.c == "q" then
				return false
			elseif e.key.sym == event.KEY_UP then
				zoom = 1
			elseif e.key.sym == event.KEY_DOWN then
				zoom = -1
			elseif e.key.sym == event.KEY_LEFT then
				rotate = -1
			elseif e.key.sym == event.KEY_RIGHT then
				rotate = 1
			end
        elseif e.type == event.KEYUP then
			if e.key.sym == event.KEY_UP or e.key.sym == event.KEY_DOWN then
				zoom = 0
			elseif e.key.sym == event.KEY_LEFT
			or e.key.sym == event.KEY_RIGHT then
				rotate = 0
			end
        elseif e.type == event.TOUCHDOWN then
            if e.touch.dx < 240 then
				rotate = -1
            else
                rotate = 1
            end
        end
		e = event.poll()
    end

    rot = rot + rotate
    if rot > 359 then rot = 0 end

	i = i + zoom

    --render.bind_buffers(state)
    render.bind_depthbuffer(state)
    render.clear(0.0, 0.0, 0.0)
	render.load_identity()
	--render.rotate(-90, 0, 0, 1)
	--render.render_elements(m.vertex, m.vertices, m.face, m.faces)
	render.color(1.0, 1.0, 1.0)
	render.translate(0, 0, i)
	render.rotate(-90, 1, 0, 0)
	render.rotate(rot, 0, 0, 1)
	--[[
    render.enable_texcoords(mesh.uv)
    render.bind_texture(tex)
	render.render_triangles(mesh.vertex, mesh.vertices, mesh.triangle, mesh.triangles)
    --render.render_quad(10)
    render.disable_texcoords()
    render.bind_texture(0)]]--
	
	md2.tick(b, event.time() * 0.001)
	md2.setup_render(b)
	md2.render(b)
	md2.cleanup_render()

	render.present()
    event.sleep(1)

	return true
end

