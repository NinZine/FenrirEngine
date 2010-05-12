local p = {}
DNAStruct = p

function p.DNAStruct(type, numFields)
    local self = {}

    self.type = type
    self.numFields = numFields
    self.fields = {}
    self.length = 0 
    self.index = nil

    return self
end

function p.print(self)
    print("Struct")
    for i,v in pairs(self) do
        print("\t" .. i, v)
    end
end

return DNAStruct

