package.path = package.path .. ";./blender/?.lua"
require 'DNAField'
require 'DNAStruct'
require 'BlendFile'

function init(filename)
    local blend = BlendFile.BlendFile()
    
    BlendFile.read(blend, io.open(filename, "rb"))
    
    --printDNA(blend)
    if #blend.scenes then
        local scene = blend.scenes[1]
        buildScene(scene)
    end
    --[[for k,v in pairs(blend._blockByPointer) do
        print(k)
    end]]--
end
		
function buildScene(scene)

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
                buildMesh(object.data);
            elseif 10 == object.type  then
				print(" -> Lamp: " .. object.data.id.name)
                    break;
            elseif 11 == object.type then
                print(" -> Camera: " .. object.data.id.name)
			else
				print(" -> " .. object.type .. ": " .. object.data.id.name)
            end
        end
        
        obj = obj.next;
    end
end

function buildMesh(mesh)

    print("blender> " .. mesh.totvert);
    for i=1, mesh.totvert do
        local v = mesh.mvert[i]
        
        local x = v.co[1]
        local y = v.co[2]
        local z = v.co[3]
        
        print("blender> vertex  (" .. x .. ", " .. y .. ", " .. z .. ")")
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
        
        if mesh.mtface then
            -- UV coords are defined
            local tf = mesh.mtface[i]
            
            --print("blender> uv " .. tf.uv[1][1])
        end
    end
end

function printDNA(blend)
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

function printObject(object)
    for i, v in pairs(object) do
        print(i .. " : " .. v)
    end
end

init("./tests/drutten.blend")

