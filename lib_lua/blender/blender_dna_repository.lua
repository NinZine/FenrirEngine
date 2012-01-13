require 'blender_file_header'

local p = {}
dna_repository = p

function p.dna_repository(data, header)
    local self = {}

    self.header = header
    if data then
        p.read(self, data)
    end

    return self
end

function p.get_struct_by_type(self, type)
    return self.struct_by_type[type]
end

function p.read(self, data)
    
    if data:read(4) ~= "SDNA" then
        error("blender> Not a DNA fileblock!")
    end
    
    if data:read(4) ~= "NAME" then
        error("blender> Not a DNA fileblock!")
    end
    
    p.read_names(self, data)
    
    p.byte_align(data, 4)
    if data:read(4) ~= "TYPE" then
        error("blender> Not a DNA fileblock!")
    end
    
    p.read_types(self, data)
    
    p.byte_align(data, 4)
    if data:read(4) ~= "STRC" then
        error("blender> Invalid Blender file!")
    end
    
    p.read_structs(self, data)
end

function p.byte_align(data, count)
    if nil == count then count = 4 end
    data:seek("set", math.ceil(data:seek("cur") / count) * count)
end

function p.read_names(self, data)
    local count = blender_util.convert(data:read(4))
    
    self.names = {}
    for i=1, count do
        self.names[i] = p.read_string(data);
    end
end

function p.read_structs(self, data)
    local count = blender_util.convert(data:read(4))
    
    self.structs = {}
    self.struct_by_type = {}
    for i=1, count do
        local struct = dna_struct.dna_struct(
            blender_util.convert(data:read(2))+1,
            blender_util.convert(data:read(2)))

		print("Struct (" .. i .. "): " .. self.types[struct.type])
        
        for j=1, struct.num_fields do
            local field = dna_field.dna_field(
                blender_util.convert(data:read(2))+1,
                blender_util.convert(data:read(2))+1)
            
            field.name = self.names[field.nameIndex]
            field.type = self.types[field.typeIndex]
			if dna_field.is_pointer(field) then
				field.length = self.header.ptr_size
			else
            	field.length = self.lengths[field.typeIndex]
            end
			--field.length = self.lengths[field.typeIndex]

			if dna_field.is_array(field) then
				field.length = field.length * dna_field.num_array_items(field)
			end

            struct.fields[j] = field
			dna_field.print(field)
        end
        
        struct.index = i
		struct.length = self.lengths[struct.type]
        self.structs[i] = struct
        
        self.struct_by_type[self.types[struct.type]] = struct
		print(" size: " .. struct.length)
    end
end

function p.read_types(self, data)
    local count = blender_util.convert(data:read(4))
    
    self.types = {}
    self.lengths = {}
    
    for i=1, count do
        self.types[i] = p.read_string(data)
    end
    
    p.byte_align(data, 4);
    
    if data:read(4) ~= "TLEN" then
        error("blender> Invalid Blender file!")
    end
    
    for i=1, count do
        self.lengths[i] = blender_util.convert(data:read(2))
    end
end

function p.read_string(data)
    local s = ""
    local c = data:read(1)
    
    while c ~= "\0" do
        s = s .. c
        c = data:read(1)
    end

    return s
end

return dna_repository

