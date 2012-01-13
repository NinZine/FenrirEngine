local p = {}
blender_util = p

function p.convert(str)
  local function _b2n(exp, num, digit, ...)
    if not digit then return num end
    return _b2n(exp*256, num + digit*exp, ...)
  end
  return _b2n(256, string.byte(str, 1, -1))
end

--[[function p.convert(str)
  local function _b2n(num, digit, ...)
    if not digit then return num end
    return _b2n(num*256 + digit, ...)
  end
  return _b2n(0, string.byte(str, 1, -1))
end]]--

function p.hex(str)
	local s = ""
	for i=1, string.len(str) do
		s = s .. string.format("%.2X", str:byte(i))
	end

	return s
end

function p.convertfloat(str)
      -- Change to b4,b3,b2,b1 to unpack an LSB float
      local b4,b3,b2,b1 = string.byte(str, 1, 4)
      local exponent = (b1 % 128) * 2 + math.floor(b2 / 128)
      if exponent == 0 then return 0 end
      local sign = (b1 > 127) and -1 or 1
      local mantissa = ((b2 % 128) * 256 + b3) * 256 + b4
      mantissa = (math.ldexp(mantissa, -23) + 1) * sign
      return math.ldexp(mantissa, exponent - 127)
end

function p.get_colors(mesh)
	local colors = {}
	for k,v in pairs(mesh.mface[1]) do
		print(k)
	end
	print(string.byte(mesh.mface[1].mat_nr))

	local material = mesh.mat.next
	local materials = 0
	while material do
		table.insert(colors, material.r) 
		table.insert(colors, material.g) 
		table.insert(colors, material.b) 
		table.insert(colors, material.alpha)

		material = material.id.next
		materials = materials + 1
	end
	print(materials)

	return colors
end

function p.quad_to_tri(face, vertex)
	local v1, v2, v3, v4 = vec3.vec3(), vec3.vec3(), vec3.vec3(), vec3.vec3()
	
	v1.x,v1.y,v1.z =
		vertex[face.v1+1].co[1], vertex[face.v1+1].co[2], vertex[face.v1+1].co[3] 

	v2.x, v2.y, v2.z = vertex[face.v2+1].co[1], vertex[face.v2+1].co[2],
	vertex[face.v2+1].co[3] 
	v3.x, v3.y, v3.z = vertex[face.v3+1].co[1], vertex[face.v3+1].co[2],
	vertex[face.v3+1].co[3] 
	v4.x, v4.y, v4.z = vertex[face.v4+1].co[1], vertex[face.v4+1].co[2],
	vertex[face.v4+1].co[3] 

	local u1, u2 = vec3.sub(v1, v3), vec3.sub(v2, v4)
	local tri = {}
	if vec3.length(u1) - vec3.length(u2) > 0.0 then
		tri[1] = face.v1
		tri[2] = face.v2
		tri[3] = face.v3
		tri[4] = face.v1
		tri[5] = face.v3
		tri[6] = face.v4
	else
		tri[1] = face.v1
		tri[2] = face.v2
		tri[3] = face.v4
		tri[4] = face.v2
		tri[5] = face.v3
		tri[6] = face.v4
	end

	return tri
end


return blender_util

