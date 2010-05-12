local p = {}
DNAFieldInstance = p

function p.DNAFieldInstance(field, pointerSize)
    local self = {}

    if nil == pointerSize then pointerSize = 4 end
    self.field = field
    self.pointerSize = pointerSize

    return self
end

function p.read(self, data)
    local result = {}
    local num = DNAField.numArrayItems(self.field)
    
    for i=1, num do
        if DNAField.isPointer(self.field) then
            table.insert(result, BHeadStruct.convert(data:read(4)))
            if self.pointerSize > 4 then
                table.insert(result, BHeadStruct.convert(data:read(4)))
            end
			print("simple pointer: " .. result[i])
        elseif DNAField.isSimpleType(self.field) then
            if self.field.type == "void" then
            elseif self.field.type == "char" then
                table.insert(result, data:read(1))
            elseif self.field.type == "short" then
                table.insert(result, BHeadStruct.convert(data:read(2)))
            elseif self.field.type == "int" then
                table.insert(result, BHeadStruct.convert(data:read(4)))
            elseif self.field.type == "float" then
                table.insert(result, BHeadStruct.convert(data:read(4)))
            elseif self.field.type == "double" then
                table.insert(result, BHeadStruct.convert(data:read(8)))
            else
                error("Wrong simple type: " .. self.field.type)
            end
        end
    end

    return result
end

return DNAFieldInstance

