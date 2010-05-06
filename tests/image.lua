--img = image.load("tests/test.png")
img = image.load("test.png")

if nil == img.data then return end

print("Filename: " .. img.filename)
print("w: " .. img.w .. " h: " .. img.h .. " bpp: " .. img.bpp)

render.create_window(640, 480)
render.set_viewport(0, 0, 320, 480)
render.setup_orthogonal_view(320, 480)

texture_id = render.upload_texture(img.w, img.h, img.data)
image.free(img)

size = 10
function update()
    e = event.poll()
    while e.type ~= event.EMPTY do
        if e.key.sym == event.KEY_ESCAPE then return 1
        elseif e.key.sym == event.KEY_UP then size = size + 1
        elseif e.key.sym == event.KEY_DOWN then size = size - 1
        end

        e = event.poll()
    end

    render.clear(0,0,0)
	render.color(1,1,1)
    render.bind_texture(texture_id)
    render.render_quad(size)

    render.present()
    event.sleep(1)
end

update()

