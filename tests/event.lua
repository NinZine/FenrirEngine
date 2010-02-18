print(event.KEY_DOWN)

while true do
    e = event.poll()
    while e.type ~= event.EMPTY do
        if e.type == event.KEYDOWN then
            if e.key.c == "q" then
                return
            else
                print("keydown: " .. e.key.c)
                print("key sym: " .. e.key.sym)
            end
        end

        e = event.poll()
    end

    render.present()
    event.sleep(1)
end

