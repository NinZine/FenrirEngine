local p = {}
dna_struct = p

function p.dna_struct(type, num_fields)
    local self = {}

    self.type = type
    self.num_fields = num_fields
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

return dna_struct

