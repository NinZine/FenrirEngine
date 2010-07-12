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
    }
end

function p.get_block_by_pointer(self, pointer)
    return self.block_by_pointer[pointer];	
end

function p.get_blocks_by_dna(self, dna_idx, get_data)
    local result = {}
    local block
    for i, block in pairs(self.blocks) do
        if block.sdna_index == dna_idx and false == get_data then
            table.insert(result, block)
        elseif block.sdna_index == dna_idx and true == get_data then
            table.insert(result, block.data)
        end
    end

    return result
end

function p.parse_block(self, block, data)
    local struct = self.dna.structs[block.sdna_index]
	local block_structure = {}

    data:seek("set", block.position)

	for i=1, block.count do
		table.insert(block_structure, p.read_struct(self, data, struct))
		assert(data:seek() == block.position + (struct.length * i), 
			"Size mismatch: " .. (block.position + (struct.length * i)) .. " ~= "
			.. data:seek())
	end

	return block_structure
end

function p.read(self, data)

    data:seek("set", 0)
    self.header = blender_file_header.file_header(data)
    self.blocks = p.read_blocks(self, data)
    self.dna = p.read_dna(self, data)

    for i, v in ipairs(self.blocks) do
		if "DNA1" ~= v.code then
			local b = p.parse_block(self, v, data)

			if b and #b == 1 then
				b = b[1]
			end
			self.pointer_data[v.pointer] = b
			v.data = b
		end
    end
	p.resolve_pointers(self)

    self.scenes = p.read_type(self, "Scene")
end

function p.read_blocks(self, data)
    local block = blender_block_header.block_header(data, self.header.ptr_size)
    local result = {}

    while block.code ~= "ENDB" do
        table.insert(result, block)
        self.block_by_pointer[block.pointer] = block
		self.pointer_data[block.pointer] = nil
        data:seek("cur", block.size) -- jump forward to next block
        block = blender_block_header.block_header(data, self.header.ptr_size)
    end

    return result
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

-- Creates DNA repository
function p.read_dna(self, data)
    local dna_block = p.sdna_block(self)
    local dna

    if dna_block then
        data:seek("set", dna_block.position)
        dna = dna_repository.dna_repository(data, self.header)
    end

    return dna
end

function p.read_field(self, data, field)
	local name = dna_field.short_name(field)
	local value = nil

	-- Pointer to either struct or C data
    if dna_field.is_pointer(field) then
		if dna_field.is_array(field) then
			value = {}
			for i=1, dna_field.num_array_items(field) do
				table.insert(value, p.read_pointer(self, data))
			end
		else
        	value = p.read_pointer(self, data)
		end
	-- C data type
    elseif dna_field.is_ctype(field) then
        local instance = dna_field_instance.dna_field_instance(field,
            self.header.ptr_size)
        value = dna_field_instance.read(instance, data)
        
        if field.type == "char" and dna_field.is_array(field) then
            value = table.concat(value)
        elseif value and #value == 1 then
			value = value[1]
		end
    else
		-- Abstract data type (struct)
        local struct = dna_repository.get_struct_by_type(self.dna, field.type)
        if struct then
			-- Struct laid out flat in this field
			if dna_field.is_array(field) then
				value = {}
				for i=1, dna_field.num_array_items(field) do
					table.insert(value, p.read_struct(self, data, struct))
				end
			else
				value = p.read_struct(self, data, struct)
			end
        else
			print("blender> unknown data")
            data:seek("cur", field.length)
        end
	end

	return name, value
end

function p.read_pointer(self, data)
    local s = "#" .. blender_util.hex(data:read(4))
    if self.header.ptr_size > 4 then
        s = s .. blender_util.hex(data:read(4))
    end

	if "#00000000" == s or "#0000000000000000" == s then s = nil end

    return s
end

function p.read_struct(self, data, struct)
    local tmp = {}
	local position = data:seek() 
    
	--print("Struct: " .. self.dna.types[struct.type])
    for i, field in ipairs(struct.fields) do
		local name, value = p.read_field(self, data, field)
		tmp[name] = value
		--print(name, value)

		position = position + field.length
		assert(data:seek() == position,
			"Field mismatch: " .. data:seek() .. " ~= " .. position ..
			"\nName: " .. field.name .. "Length: " .. field.length)
    end
    
    return tmp
end

function p.read_type(self, type)
    local struct = dna_repository.get_struct_by_type(self.dna, type)
    return p.get_blocks_by_dna(self, struct.index, true)
end

function p.resolve_pointers(self)
	local function resolve(field)
		if type(field) == "string" and "#" == string.sub(field, 1, 1) then
			return self.pointer_data[field]
		elseif type(field) == "table" then
			for k, f in pairs(field) do
				field[k] = resolve(f)
			end
			return field
		else
			return field
		end
	end
	for i, b in ipairs(self.blocks) do
		if b.data ~= nil then
			for key, field in pairs(b.data) do
				b.data[key] = resolve(field)
			end
		end
	end
end

function p.sdna_block(self)
    
    for i,block in pairs(self.blocks) do
        if block.code == "DNA1" then
            return block
        end
    end
    
    return nil
end

