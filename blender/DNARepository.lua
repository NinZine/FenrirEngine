require 'BlendFileHeader'

local p = {}
DNARepository = p

function p.DNARepository(data, header)
    local self = {}

    self.header = header
    if data then
        p.read(self, data)
    end

    return self
end

function p.getStructByType(self, type)
    return self._structByType[type]
end

function p.read(self, data)
    
    if data:read(4) ~= "SDNA" then
        error("Not a DNA fileblock!")
    end
    
    if data:read(4) ~= "NAME" then
        error("Not a DNA fileblock!")
    end
    
    p.readNames(self, data)
    
    p.byteAlign(data, 4)
    if data:read(4) ~= "TYPE" then
        error("Not a DNA fileblock!")
    end
    
    p.readTypes(self, data)
    
    p.byteAlign(data, 4)
    if data:read(4) ~= "STRC" then
        error("Invalid Blender file!")
    end
    
    p.readStructs(self, data)
end

function p.byteAlign(data, count)
    if nil == count then count = 4 end
    data:seek("set", math.ceil(data:seek("cur") / count) * count)
end

function p.readNames(self, data)
    local count = BHeadStruct.convert(data:read(4))
    
    self.names = {}
    for i=1, count do
        self.names[i] = p.readString(data);
    end
end

function p.readStructs(self, data)
    local count = BHeadStruct.convert(data:read(4))
    
    self.structs = {}
    self._structByType = {}
    for i=1,count do
        local struct = DNAStruct.DNAStruct(
            BHeadStruct.convert(data:read(2))+1,
            BHeadStruct.convert(data:read(2)))

		print("Struct: " .. self.types[struct.type])
        
        for j=1, struct.numFields do
            local field = DNAField.DNAField(
                BHeadStruct.convert(data:read(2))+1,
                BHeadStruct.convert(data:read(2))+1)
            
            field.name = self.names[field.nameIndex]
            field.type = self.types[field.typeIndex]
            field.length = self.lengths[field.typeIndex]
            
            struct.fields[j] = field
            struct.length = struct.length + field.length

			DNAField.print(field)
        end
        
        struct.index = i
        self.structs[i] = struct
        
        self._structByType[self.types[struct.type]] = struct
    end
end

function p.readTypes(self, data)
    local count = BHeadStruct.convert(data:read(4))
    
    self.types = {}
    self.lengths = {}
    
    for i=1, count do
        self.types[i] = p.readString(data)
    end
    
    p.byteAlign(data, 4);
    
    if data:read(4) ~= "TLEN" then
        error("Invalid Blender file!")
    end
    
    for i=1, count do
        self.lengths[i] = BHeadStruct.convert(data:read(2))
    end
end

function p.readString(data)
    local s = ""
    local c = data:read(1)
    
    while c ~= "\0" do
        s = s .. c
        c = data:read(1)
    end

    return s
end

return DNARepository

