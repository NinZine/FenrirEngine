--img = image.load("tests/test.png")
img = image.load("./tests/tileset.png")

if nil == img.data then return end

print("Filename: " .. img.filename)
print("w: " .. img.w .. " h: " .. img.h .. " bpp: " .. img.bpp)

render.create_window(640, 480)
render.setup_orthogonal_view(320, 240)

texture_id = render.upload_texture(img.w, img.h, img.data)
coords = render.generate_texcoords(img.w, img.h, 32, 32)
subimages = img.w/32 * img.h/32
print(subimages)

size = 10
while true do
    e = event.poll()
    while e.type ~= event.EMPTY do
        if e.key.sym == event.KEY_ESCAPE then
            image.free(img)
            return
        elseif e.key.sym == event.KEY_UP then size = size + 1
        elseif e.key.sym == event.KEY_DOWN then size = size - 1
        end

        e = event.poll()
    end

    render.clear(1,1,1)
	render.load_identity()
    render.bind_texture(texture_id)
    render.enable_blending()
    --render.translate(-img.w/4, img.h/4, 0)
	for i=0, subimages-1 do
        render.push_matrix()
    	render.translate((i % 32) * size, -math.floor((i / 32)) * size, 0)
		render.render_texquad(size, coords, i)
        render.pop_matrix()
	end
    render.disable_blending()

    render.present()
    event.sleep(1)
end

