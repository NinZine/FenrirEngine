b = blender.open("drutten.blend")
m = blender.get_mesh("Drutten", b)
n = blender.get_mesh("Hat", b)
blender.close(b)
--[[m = model.open_cex("/Users/ninzine/Code/Lua/ConceptEngine/tests/export.cex")
]]--

--state = render.state()
rot = 0
render.create_window(320, 480)
--render.setup_orthogonal_view(800, 600)
render.setup_perspective_view(60, 320/480, 0.01, 90.0)
local i = -10
local dir = 1
function update()
    e = event.poll()
    while e.type ~= event.EMPTY do
        if e.type == event.KEYDOWN then
            if e.key.c == "q" then
				return
			elseif e.key.sym == event.KEY_UP then
				i = i + 0.2
			elseif e.key.sym == event.KEY_DOWN then
				i = i - 0.2
			elseif e.key.sym == event.KEY_LEFT then
				rot = rot - 1
			elseif e.key.sym == event.KEY_RIGHT then
				rot = rot + 1
			end
        end
		e = event.poll()
    end

	if i < -10 then dir = 1
	elseif i > 0 then dir = -1
	end

	i = i + dir

    --render.bind_buffers(state)
    render.clear(0.0, 0.0, 0.0)
	render.load_identity()
	render.rotate(-90, 0, 0, 1)
	--render.render_elements(m.vertex, m.vertices, m.face, m.faces)
	render.color(1.0, 1.0, 1.0)
	render.translate(0, 0, i)
	render.rotate(rot, 0, 1, 0)
    render.render_mesh(m)
    render.render_mesh(n)
	render.present()
    event.sleep(1)
end

