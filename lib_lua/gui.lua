local p = {}
gui = p

local function build_corner(x, y, start_angle, end_angle, radius, smooth)
	local v = {}
	local step = (end_angle - start_angle) / smooth
    local slice = 0

    local i=0 -- Start outside
    local current_slice = 0
	while i < smooth*2 do
		if i % 3 == 1 then
            slice = slice + 1
            table.insert(v, x)
            table.insert(v, y)
            table.insert(v, 0) -- Get rid of
		else
			local angle = start_angle + step * current_slice 
			table.insert(v, x + math.cos(angle) * radius)
			table.insert(v, y + math.sin(angle) * radius)
			table.insert(v, 0) -- Get rid of
            current_slice = current_slice + 1
		end
        i = i + 1
	end
    return v
end

-- TODO: Return texture 
function p.rounded_box(x, y, width, height, radius)
	--[[ Find corners where roundness begins (top left, bottom left, top right,
		bottom right) ]]--
	local corner = {
		top = height/2 - radius + y,
		bottom = (-height/2) + radius + y,
		right = width/2 - radius + x,
		left = (-width/2) + radius + x
	}

	local smooth = 6
	local vertex = {}

	for i,v in ipairs(build_corner(corner.left, corner.top, math.pi, math.pi/2,
        radius, smooth)) do
		table.insert(vertex, v)
	end
    -- Build top quad
    table.insert(vertex, corner.left)
    table.insert(vertex, corner.top)
    table.insert(vertex, 0)
    table.insert(vertex, corner.left)
    table.insert(vertex, corner.top + radius)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right)
    table.insert(vertex, corner.top)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right)
    table.insert(vertex, corner.top + radius)
    table.insert(vertex, 0)

	for i,v in ipairs(build_corner(corner.right, corner.top, math.pi/2, 0,
        radius, smooth)) do
		table.insert(vertex, v)
	end
    -- Build right quad
    table.insert(vertex, corner.right)
    table.insert(vertex, corner.top)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right + radius)
    table.insert(vertex, corner.top)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right)
    table.insert(vertex, corner.bottom)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right + radius)
    table.insert(vertex, corner.bottom)
    table.insert(vertex, 0)

	for i,v in ipairs(build_corner(corner.right, corner.bottom, 0, -math.pi/2,
        radius, smooth)) do
		table.insert(vertex, v)
	end
    -- Build bottom quad
    table.insert(vertex, corner.right)
    table.insert(vertex, corner.bottom)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right)
    table.insert(vertex, corner.bottom - radius)
    table.insert(vertex, 0)
    table.insert(vertex, corner.left)
    table.insert(vertex, corner.bottom)
    table.insert(vertex, 0)
    table.insert(vertex, corner.left)
    table.insert(vertex, corner.bottom - radius)
    table.insert(vertex, 0)

	for i,v in ipairs(build_corner(corner.left, corner.bottom, -math.pi/2,
        -math.pi, radius, smooth)) do
		table.insert(vertex, v)
	end
    -- Build left quad
    table.insert(vertex, corner.left)
    table.insert(vertex, corner.bottom)
    table.insert(vertex, 0)
    table.insert(vertex, corner.left - radius)
    table.insert(vertex, corner.bottom)
    table.insert(vertex, 0)
    table.insert(vertex, corner.left)
    table.insert(vertex, corner.top)
    table.insert(vertex, 0)
    table.insert(vertex, corner.left - radius)
    table.insert(vertex, corner.top)
    table.insert(vertex, 0)
    -- Build central quad
    table.insert(vertex, corner.left - radius)
    table.insert(vertex, corner.bottom)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right + radius)
    table.insert(vertex, corner.top)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right + radius)
    table.insert(vertex, corner.bottom)
    table.insert(vertex, 0)

	local vertices = table.maxn(vertex) / 3
	vertex = util.to_float(vertex)

	render.render_vertices(vertex, vertices)
end

return p

