require 'bit'

local p = {}

function p:color_components(x, y)
	local c = image.get_pixel(self.image, x, y)
	local color = {
		r = bit.band(0x000000FF, c) / 255,
		g = bit.brshift(bit.band(0x0000FF00, c), 8) / 255,
		b = bit.brshift(bit.band(0x00FF0000, c), 16) / 255,
		a = bit.brshift(bit.band(0xFF000000, c), 24) / 255
	}
	
	return c
end

function p:create(filename)
    local m = image.load(filename)
    if nil == m.data then return end

    print("tilemap> filename: " .. m.filename)
    print("\tw: " .. m.w .. " h: " .. m.h .. " bpp: " .. m.bpp .. " type: " ..
		m.type)
    print("\t(0,0) = " .. image.get_pixel(m, 0, 0))

	image.flip_y(m)

	local s = {
		image = m
	}
	setmetatable(s, self)
	self.__index = self

	return s
end

function p:delete()
	image.free(self.image)
end

function p:render(position, radius)
	local p = {
		sx = position.x - radius,
		sy = position.y - radius
	}

	if p.sx < 0 then p.sx = 0 end
	if p.sy < 0 then p.sy = 0 end

	p.dx, p.dy = p.sx + (2 * radius), p.sy + (2 * radius)

	if p.dx >= self.image.w then p.dx = self.image.w - 1 end
	if p.dy >= self.image.w then p.dy = self.image.w - 1 end

	if position.x > 0 and position.y > 0 then
		hud.draw_text(string.format("0x%x", image.get_pixel(self.image,
			position.x, position.y)), 180, -150)
	end

    render.push_matrix()
	render.translate(0, 0, -0.18)
    for y=p.sy, p.dy do
        render.push_matrix()
        for x=p.sx, p.dx do
           	if 0xFF000000 ~= image.get_pixel(self.image, x, y) then
				render.color(0, 0.8, 0.2)
				render.render_quad(1)
				render.translate(1, 0, 0)
			end
        end
        render.pop_matrix()
        render.translate(0, 1, 0)
    end
    render.pop_matrix()
end

tilemap = p
return p
