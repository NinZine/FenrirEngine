local p = {}
BlendFileHeader = p

local MAGIC = 'BLENDER';
local ENDIAN_BIG = 'V';
local ENDIAN_LITTLE = 'v';
local POINTERSIZE_4 = '_';
local POINTERSIZE_8 = '-';

function p.BlendFileHeader(data)
    local self = {}
    data:seek("set", 0)
    
    if data:read(7) ~= MAGIC then
        error("Not a Blender .blend file!")
    end
    
    local b = data:read(1)
    if POINTERSIZE_4 == b then
        self.ptr_size = 4
    elseif POINTERSIZE_8 == b then
            self.ptr_size = 8
    else
        error("Not a Blender .blend file!")
    end
    
    b = data:read(1)
    self.endian = b
    if ENDIAN_BIG ~= b and ENDIAN_LITTLE ~= b then
        error("Not a Blender .blend file!")
    end
    
    self.version = data:read(3)
    return self
end

return BlendFileHeader

