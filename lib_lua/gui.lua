local p = {}
gui = p

local function build_corner(x, y, start_angle, end_angle, radius, smooth)
	local v = {}
	local index = {}
	local step = (end_angle - start_angle) / smooth
    local slice = 0

	-- Vertex in the middle of a circle
	table.insert(v, x)
	table.insert(v, y)
	table.insert(v, 0)

	table.insert(v, x + math.cos(start_angle) * radius)
	table.insert(v, y + math.sin(start_angle) * radius)
	table.insert(v, 0)

    local i=0
    local current_slice = 1
	while current_slice < smooth+1 do
		if i % 3 == 0 then
            slice = slice + 1
			table.insert(index, 0)
		else
			if i % 3 == 2 then
				local angle = start_angle + step * current_slice 
				table.insert(v, x + math.cos(angle) * radius)
				table.insert(v, y + math.sin(angle) * radius)
				table.insert(v, 0)
				current_slice = current_slice + 1
			end
			table.insert(index, current_slice)
		end
        i = i + 1
	end
    return index, v
end

local function maxn(i)
	local m = 0
	for i,v in ipairs(i) do
		if m < v then
			m = v
		end
	end
	return m
end

local function merge_vertices(vertex1, vertex2)
	for j, u in ipairs(vertex2) do
		table.insert(vertex1, u)
	end

	return vertex1
end

local function merge_indices(index1, index2)
	local largest = maxn(index1)
	if largest > 0 then largest = largest + 1 end
	for j, u in ipairs(index2) do
		table.insert(index1, u+largest)
	end

	return index1
end


-- TODO: Return texture or cache in some way
function p.rounded_box(x, y, width, height, radius)
	--[[ Find corners where roundness begins (top left, bottom left, top right,
		bottom right) ]]--
	local corner = {
		top = height/2 - radius + y,
		bottom = (-height/2) + radius + y,
		right = width/2 - radius + x,
		left = (-width/2) + radius + x
	}

	local smooth = 4
	local vertex, index = {}, {}

	-- Build corners
	local i,v = build_corner(corner.left, corner.top, math.pi/2, math.pi,
        radius, smooth)
	vertex = merge_vertices(vertex, v)
	index = merge_indices(index, i)

	i,v = build_corner(corner.right, corner.top, 0, math.pi/2,
        radius, smooth)
	vertex = merge_vertices(vertex, v)
	index = merge_indices(index, i)

	i,v = build_corner(corner.right, corner.bottom, -math.pi/2, 0,
        radius, smooth)
	vertex = merge_vertices(vertex, v)
	index = merge_indices(index, i)

	i,v = build_corner(corner.left, corner.bottom, -math.pi, -math.pi/2,
        radius, smooth)
	vertex = merge_vertices(vertex, v)
	index = merge_indices(index, i)

	-- Build quads
    table.insert(vertex, corner.left)
    table.insert(vertex, corner.top + radius)
    table.insert(vertex, 0)
    table.insert(vertex, corner.left)
    table.insert(vertex, corner.bottom - radius)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right)
    table.insert(vertex, corner.bottom - radius)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right)
    table.insert(vertex, corner.top + radius)
    table.insert(vertex, 0)

	local maxi = maxn(index) + 1
	table.insert(index, maxi)
	table.insert(index, maxi + 1)
	table.insert(index, maxi + 2)
	table.insert(index, maxi + 2)
	table.insert(index, maxi + 3)
	table.insert(index, maxi)

    table.insert(vertex, corner.left - radius)
    table.insert(vertex, corner.top)
    table.insert(vertex, 0)
    table.insert(vertex, corner.left - radius)
    table.insert(vertex, corner.bottom)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right + radius)
    table.insert(vertex, corner.bottom)
    table.insert(vertex, 0)
    table.insert(vertex, corner.right + radius)
    table.insert(vertex, corner.top)
    table.insert(vertex, 0)

	maxi = maxn(index) + 1
	table.insert(index, maxi)
	table.insert(index, maxi + 1)
	table.insert(index, maxi + 2)
	table.insert(index, maxi + 2)
	table.insert(index, maxi + 3)
	table.insert(index, maxi)


	local vertices = table.maxn(vertex) / 3
	vertex = util.to_float(vertex)
	local faces = table.maxn(index) / 3
	index = util.to_uint16(index)

	render.push_matrix()
	render.render_triangles(vertex, vertices, index, faces)
	render.pop_matrix()
end

return p

