require 'blender_util'

local p = {}
blender_block_header = p

function p.block_header(data, pointer_size)
    local self = {}
    local p = {}

    self.code = data:read(4)
    self.size = blender_util.convert(data:read(4))
    self.pointer = "#" .. blender_util.hex(data:read(4))
    if pointer_size ~= 4 then
        self.pointer = self.pointer .. blender_util.hex(data:read(4))
    end
    self.sdna_index = blender_util.convert(data:read(4))+1
    self.count = blender_util.convert(data:read(4))
    self.position = data:seek("cur")
    
    print(string.format("code:%s size:%d ptr:%s sdna:%d", self.code,
        self.size,
        self.pointer,
        self.sdna_index))
    --[[for k,v in pairs(self) do
        print(k, v)
    end]]--

	self.data = nil

    return self
end

function p.print(self)
    print("Block")
    for i,v in pairs(self) do
        print("\t" .. i, v)
    end
end

return blender_block_header

