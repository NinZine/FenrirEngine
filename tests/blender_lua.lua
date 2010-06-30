local b = blender.load("tests/drutten.blend")
local obj = blender.get_object(b, "Drutten.001")
local mesh = blender.build_mesh(obj.data)
--local mesh = blender.build_scene(b.scenes[1])

--blender.close(b)
--[[m = model.open_cex("/Users/ninzine/Code/Lua/ConceptEngine/tests/export.cex")
]]--

--state = render.state()
render.create_window(320, 480)
--render.setup_orthogonal_view(800, 600)
render.setup_perspective_view(60, 320/480, 0.01, 90.0)
render.set_viewport(0, 0, 320, 480)
local i = -10
local dir = 1
local rotate = 0
local rot = 0
function update()
    e = event.poll()
    while e.type ~= event.EMPTY do
        if e.type == event.KEYDOWN then
            if e.key.c == "q" then
				return false
			elseif e.key.sym == event.KEY_UP then
				i = i + 0.2
			elseif e.key.sym == event.KEY_DOWN then
				i = i - 0.2
			elseif e.key.sym == event.KEY_LEFT then
				rot = rot - 1
			elseif e.key.sym == event.KEY_RIGHT then
				rot = rot + 1
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

    --render.bind_buffers(state)
    render.clear(0.0, 0.0, 0.0)
	render.load_identity()
	render.rotate(-90, 0, 0, 1)
	--render.render_elements(m.vertex, m.vertices, m.face, m.faces)
	render.color(1.0, 1.0, 1.0)
	render.translate(0, 0, i)
	render.rotate(-90, 1, 0, 0)
	render.rotate(rot, 0, 1, 0)
	render.render_triangles(mesh.vertex, mesh.vertices, mesh.triangle, mesh.triangles)
	render.present()
    event.sleep(1)

	return true
end

