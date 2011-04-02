local state = render.state()
render.create_window(320, 480)
render.setup_orthogonal_view(320, 480)
--render.setup_perspective_view(60, 320/480, 0.01, 500.0)
render.set_viewport(0, 0, 320, 480)
state.depth = render.generate_depthbuffer(320, 480)
--render.enable_culling(1)

local f = font.load("tests/04b_03.ttf", 8)

function update()
    e = event.poll()
    while e.type ~= event.EMPTY do
        if e.type == event.KEYDOWN then
            if e.key.c == "q" then
				return false
			end
        end
		e = event.poll()
    end

    --render.bind_buffers(state)
    render.bind_depthbuffer(state)
    render.clear(0.5, 0.5, 0.5)
	render.load_identity()
	--render.rotate(-90, 0, 0, 1)
    --render.translate(0, 0, -5)

	render.disable_depth();
	render.color(0.0, 0.0, 0.0)
    font.draw_string(f, "HEY!\nwhat's up?", 1, -1)
	render.color(1.0, 1.0, 1.0)
    font.draw_string(f, "HEY!\nwhat's up?", 0, 0)

	render.present()
    event.sleep(1)

	return true
end

