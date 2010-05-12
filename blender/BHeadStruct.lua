local p = {}
BHeadStruct = p

function p.BHeadStruct(data, pointerSize)
    local self = {}

    self.code = data:read(4)
    self.size = p.convert(data:read(4))
    self.pointer = p.convert(data:read(4))
    if pointerSize ~= 4 then
        self.pointer = self.pointer + p.convert(data:read(4))
    end
    self.sdnaIndex = p.convert(data:read(4))+1
    self.count = p.convert(data:read(4))
    self.position = data:seek("cur")
    
    print(string.format("size:%d ptr:%d sdna:%d", self.size,
        self.pointer,
        self.sdnaIndex))
    --[[for k,v in pairs(self) do
        print(k, v)
    end]]--

    return self
end

function p.convert(str)
  local function _b2n(exp, num, digit, ...)
    if not digit then return num end
    return _b2n(exp*256, num + digit*exp, ...)
  end
  return _b2n(256, string.byte(str, 1, -1))
end

function p.print(self)
    print("Block")
    for i,v in pairs(self) do
        print("\t" .. i, v)
    end
end

return BHeadStruct

