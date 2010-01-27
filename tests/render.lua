state = render.new_state()
--state.framebuffer = render.generate_framebuffer()
--state.renderbuffer = render.generate_renderbuffer()
--state.depthbuffer = render.generate_depthbuffer()

--render.bind_buffers(state)
--render.setup_orthogonal_view()
local i = 0.01
while true do
    e = event.poll()
    while e.type ~= event.EMPTY do
        if e.type == event.KEYDOWN then
            if e.key.c == "q" then return end
        end
    end

    --render.bind_buffers(state)
    render.clear(0.0, 1.0, 0.0)
    render.render_quad(0.5)
    render.present()
    event.sleep(1)
end

