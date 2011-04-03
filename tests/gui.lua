string.split = function(str, pattern)
	pattern = pattern or "[^%s]+"
	if pattern:len() == 0 then pattern = "[^%s]+" end
	local parts = {__index = table.insert}
	setmetatable(parts, parts)
	str:gsub(pattern, parts)
	setmetatable(parts, nil)
	parts.__index = nil
	return parts
end

local p = string.split(package.path, "[^%?]+")
package.path = package.path .. ";" .. p[1] .. "lib_lua/?.lua" .. ";"
require 'gui'


local state = render.state()
render.create_window(320, 480)
render.setup_orthogonal_view(320, 480)
--render.setup_perspective_view(60, 320/480, 0.01, 500.0)
render.set_viewport(0, 0, 320, 480)
--state.depth = render.generate_depthbuffer(320, 480)
--render.enable_culling(1)

local f = font.load("tests/04b_03.ttf", 8)
local height = 40
local animate = 0

function update()
    e = event.poll()
    while e.type ~= event.EMPTY do
        if e.type == event.KEYDOWN then
            if e.key.c == "q" then
				return false
			end
        elseif e.type == event.TOUCHDOWN then
            if height == 40 then
                animate = 5
                height = height + animate
            elseif height == 80 then
                animate = -5
                height = height + animate
            end
        end
		e = event.poll()
    end
    

    if height > 40 and height < 80 then
        height = height + animate
    end

    --render.bind_buffers(state)
    --render.bind_depthbuffer(state)
    render.clear(0.5, 0.5, 0.5)
	render.load_identity()
	--render.rotate(-90, 0, 0, 1)
    --render.translate(0, 0, -5)

    render.push_matrix()
    render.translate(0, -2, 0)
    render.color(0.2, 0.2, 0.2)
    gui.rounded_box(0, -(height/2-20), 144, height + 4, 12)
    render.pop_matrix()

    render.color(1, 1, 1)
    gui.rounded_box(0, -(height/2-20), 144, height + 4, 12)
    
    render.color(0.2, 0.2, 0.8)
    gui.rounded_box(0, -(height/2-20), 140, height, 10)
	
    render.disable_depth();
	render.color(0.0, 0.0, 0.0)
    render.translate(-60, 5, 0)
    font.draw_string(f, "HEY!\nwhat's up?", 1, -1)
	render.color(1.0, 1.0, 1.0)
    font.draw_string(f, "HEY!\nwhat's up?", 0, 0)

	render.present()
    event.sleep(1)

	return true
end

