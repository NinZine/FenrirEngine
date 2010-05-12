local p = {}
DNAField = p

function p.DNAField(typeIndex, nameIndex)
    local self = {}

    self.typeIndex = typeIndex
    self.nameIndex = nameIndex	
    self.type = ""
    self.name = ""
    self.length = nil
    self.arrayItems = {}

    return self
end

function p.numArrayItems(self)
    local num = 1
    
    self.arrayItems = {}
    
    if p.isArray(self) then
        local s = string.find(self.name, "[", 1, true)
        local e = string.find(self.name, "]", 1, true)

        if s ~= nil and e ~= nil then
            num = tonumber(string.sub(self.name, s+1, e-1))
            table.insert(self.arrayItems, num)
            
            local t = string.sub(self.name, e+1)
            s = string.find(t, "[", 1, true)
            e = string.find(t, "]", 1, true)
            if s ~= nil and e ~= nil then
                local n = string.sub(t, s+1, e-1)
                num = num * n;
                table.insert(self.arrayItems, n)
            end
        end
    end
    
    return num
end

function p.isArray(self)
    return (string.find(self.name, "[", 1, true) ~= nil and
        string.find(self.name, "]", 1, true) ~= nil)
end

function p.isCType(self)
    return (
        self.type == "void" or
        self.type == "char" or
        self.type == "short" or
        self.type == "int" or
        self.type == "float" or
        self.type == "double"
    )
end

function p.isPointer(self)
    return (string.sub(self.name, 1, 1) == "*")
end

function p.isSimpleType(self)
    if p.isPointer(self) then return true end
    return p.isCType(self)
end

function p.shortName(self)
    local name = self.name
    
    while string.sub(name, 1, 1) == "*" do
        name = string.sub(name, 2)
    end
    
    local parts = string.find(name, "[", 1, true)
    if parts ~= nil then
        name = string.sub(name, 1, parts-1)
    end
    
    return name
end

function p.print(self)
	print("\t" .. self.name .. " (" .. self.type .. ")")
end

return DNAField

