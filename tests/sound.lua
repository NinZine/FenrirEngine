--img = image.load("tests/test.png")
house = sound.ogg_open("./tests/house.ogg")

render.create_window(640, 480)
render.setup_orthogonal_view(320, 240)

size = 10
sound.ogg_play(house)
while true do
    e = event.poll()
    while e.type ~= event.EMPTY do
        if e.key.sym == event.KEY_ESCAPE then return
        elseif e.key.sym == event.KEY_UP then size = size + 1
        elseif e.key.sym == event.KEY_DOWN then size = size - 1
        end

        e = event.poll()
    end

    if false == sound.ogg_update(house) then
        return
    end

    render.clear(0,0,0)
    render.render_quad(size, nil)

    render.present()
    event.sleep(1)
end

