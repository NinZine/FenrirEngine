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

local pa = string.split(package.path, "[^%?]+")
package.path = package.path .. ";" .. pa[1] .. "blender/?.lua"
require 'blender_dna_field'
require 'blender_dna_struct'
require 'blender_file'

local p = {
	block_header = blender_block_header,
	dna_field = blender_dna_field,
	dna_field_instance = blender_dna_field_instance,
	dna_repository = blender_dna_repository,
	dna_struct = blender_dna_struct,
	file = blender_file,
	file_header = blender_file_header,
}
blender = p

function p.load(filename)
    local blend = p.file.file()
    
    p.file.read(blend, io.open(filename, "rb"))
    
    --printDNA(blend)
    if #blend.scenes then
        local scene = blend.scenes[1]
        --p.build_scene(scene)
    end
    --[[for k,v in pairs(blend._blockByPointer) do
        print(k)
    end]]--

	return blend
end
		
function p.build_scene(scene)
    local obj = scene.base.first
    
    for i,v in ipairs(scene.base) do
        print(i,v)
    end

    while obj do
        local object = obj.object 
        
        print("Object name: " .. object.id.name .. " type: " .. object.type ..
            " matrix: " .. object.obmat[1])
        
        --printObject(object)
        
        if object.data then
            if 1 == object.type then
                print(" -> Mesh: " .. object.data.id.name)
                p.build_mesh(object.data)
            elseif 10 == object.type  then
				print(" -> Lamp: " .. object.data.id.name)
            elseif 11 == object.type then
                print(" -> Camera: " .. object.data.id.name)
			else
				print(" -> " .. object.type .. ": " .. object.data.id.name)
            end
        end
        
        obj = obj.next;
    end
end

function p.build_mesh(mesh)
	local m = {}
	local vertices = {}
	local triangles = {}
	--local colors = blender_util.get_colors(mesh)

    print("blender> " .. mesh.totvert);
    for i=1, mesh.totvert do
        local v = mesh.mvert[i]
        
        local x = v.co[1]
        local y = v.co[2]
        local z = v.co[3]
        
        print("blender> vertex  (" .. x .. ", " .. y .. ", " .. z .. ")")
		table.insert(vertices, x)
		table.insert(vertices, y)
		table.insert(vertices, z)
		print(string.byte(v.mat_nr))
    end
    
    print("blender> faces " .. mesh.totface)
    
    for i=1, mesh.totface do
        local f = mesh.mface[i]
        
        local v1 = f.v1
        local v2 = f.v2
        local v3 = f.v3
        local v4 = f.v4
        
        print("blender> indices (" .. v1 .. ", " .. v2 .. ", " .. v3
			.. ", " .. v4 .. ")")
		print(string.byte(f.mat_nr))

		if v4 > 0 then
			local tri = blender_util.quad_to_tri(f, mesh.mvert)
			for j=1, 6 do
				table.insert(triangles, tri[j])
			end
		else
			table.insert(triangles, v1)
			table.insert(triangles, v2)
			table.insert(triangles, v3)
		end
        
        if mesh.mtface then
            -- UV coords are defined
            local tf = mesh.mtface[i]
            
            --print("blender> uv " .. tf.uv[1][1])
        end
    end

	for i=1, table.maxn(triangles), 3 do
		print("blender> triangle (" .. triangles[i] .. ", " .. triangles[i+1] ..
		", " .. triangles[i+2] .. ")")
	end


	m.vertex = util.to_float(vertices)
	m.triangle = util.to_uint16(triangles)
	m.vertices = table.maxn(vertices)
	m.triangles = table.maxn(triangles) / 3
	--m.colors = table.maxn(colors) / 4
	--m.color = util.to_float(colors)
	
	return m
end

function p.get_object(blend, name)
	local function trim(s)
		return string.match(s, "^[%w_%.]+")
	end

	local obj = blend.scenes[1].base.first
	local tmpname = "OB" .. name
	local len = string.len(tmpname)

	while obj do
		local object = obj.object
		local trimname = trim(object.id.name)
		if trimname == tmpname then
			return object
		end
		obj = obj.next
	end

	return obj
end

function p.print_dna(blend)
    local struct
    local field
    
    for i, struct in pairs(blend.dna.structs) do
        local type = blend.dna.types[struct.type]
        
        print(type)
        
        for j, field in pairs(struct.fields) do
            print(" -> " .. field.type .. " " .. field.name)
        end
    end
end

function p.print_object(object)
    for i, v in pairs(object) do
        print(i .. " : " .. v)
    end
end

--p.load("./tests/drutten.blend")

return blender
