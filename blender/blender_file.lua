require 'blender_file_header'
require 'blender_block_header'
require 'blender_dna_field_instance'
require 'blender_dna_field'
require 'blender_dna_struct'
require 'blender_dna_repository'

local p = {}
blender_file = p


function p.file() 
	return {
        block_by_pointer = {},
        pointer_data = {},
        read_pointers = {},
    }
end

function p.get_block_by_pointer(self, pointer)
    return self.block_by_pointer[pointer];	
end

function p.get_blocks_by_dna(self, dna_idx)
    local result = {}
    local block
    for i, block in pairs(self.blocks) do
        if block.sdna_index == dna_idx then
            table.insert(result, block)	
        end
    end

    return result
end

function p.read(self, data)

    data:seek("set", 0)
    self.header = blender_file_header.file_header(data);

    self.blocks = p.read_blocks(self, data);
    self.dna = p.read_dna(self, data);
    self.scenes = p.read_type(self, data, "Scene");
end

function p.read_type(self, data, type)
    local struct = dna_repository.get_struct_by_type(self.dna, type)
    local blocks = p.get_blocks_by_dna(self, struct.index)
    local result = {}

    for i,v in ipairs(blocks) do
        blender_block_header.print(v)
        table.insert(result, p.read_block(self, data, v))
    end
    
    return result
end

function p.read_blocks(self, data)
    local block = blender_block_header.block_header(data, self.header.ptr_size)
    local result = {}

    while block.code ~= "ENDB" do
        table.insert(result, block)
        self.block_by_pointer[block.pointer] = block
        data:seek("cur", block.size) -- jump forward to next block
        block = blender_block_header.block_header(data, self.header.ptr_size)
    end

    return result
end

function p.read_dna(self, data)
    local dnaBlock = p.sdna_block(self)
    local dna

    if dnaBlock then
        data:seek("set", dnaBlock.position)
        dna = dna_repository.dna_repository(data, self.header)
    end

    return dna
end

function p.read_char_array(data)
    local s = "";
    local c
    
    c = data:read(1)
    while c ~= "\0" do
        s = s .. c
        c = data:read(1)
    end

    return s
end

function p.read_pointer(self, data)
    local s = blender_util.convert(data:read(4))
    if self.header.ptr_size > 4 then
        s = s + blender_util.convert(data:read(4))
    end

    return s
end

function p.read_block(self, data, block)
    local struct = self.dna.structs[block.sdna_index]
    local result = {}
    
	print("Struct: " .. self.dna.types[struct.type])
    dna_struct.print(struct)
    data:seek("set", block.position)
	print("block.count: " .. block.count)
    for i=1, block.count do
        local v = p.read_struct(self, data, struct)
        if block.count > 1 then
            table.insert(result, v)
        else
            result = v
        end
    end
	print("Done" .. self.dna.types[struct.type])

    return result
end

function p.read_struct(self, data, struct)
    local result = {}
    
    --print(indent+dna.types[struct.type])
    
    for i, field in ipairs(struct.fields) do
        p.read_field(self, data, field, result)
    end
    
    return result
end

function p.read_field(self, data, field, object)
    local shortName = dna_field.short_name(field)
    
	print(field.name .. "(" .. field.type .. ")")
    if dna_field.is_pointer(field) then
        local pointer = p.read_pointer(self, data)
        
		print("complex pointer: " .. pointer)
        if pointer ~= 0 and not self.read_pointers[pointer] then
            self.read_pointers[pointer] = 1

            self.pointer_data[pointer] =
                p.dereference_pointer(self, data, pointer)
            object[shortName] = self.pointer_data[pointer]
        else
            object[shortName] = self.pointer_data[pointer]
        end
    elseif dna_field.is_ctype(field) then
        local instance = dna_field_instance.dna_field_instance(field,
            self.header.ptr_size)
        local value = dna_field_instance.read(instance, data)
        
        if field.type == "char" and shortName == "name"
        and dna_field.is_array(field) then
            value = table.concat(value)
            object[shortName] = value --p.readCharArray(data)	
            print(field.name .. " = " .. object[shortName])
        else
            if value then
                if #value == 1 then
					object[shortName] = value[1]
					print(field.name .. " = " .. object[shortName])
				else object[shortName] = value
                end
            end
        end
    else
        local struct = dna_repository.get_struct_by_type(self.dna, field.type)
        if struct then
            object[shortName] = p.read_struct(self, data, struct)
        else
            data:seek("cur", field.length)
        end
    end
end

function p.dereference_pointer(self, data, pointer)
    local position = data:seek("cur")
    local block = p.get_block_by_pointer(self, pointer)
    local result
    
    if block then
        print("Block found " .. pointer)
        result = p.read_block(self, data, block)
    else
        print("Block not found " .. pointer)
    end
    
    data:seek("set", position)
    
    return result
end

function p.sdna_block(self)
    
    for i,block in pairs(self.blocks) do
        if block.code == "DNA1" then
            return block
        end
    end
    
    return nil
end

