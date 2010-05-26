require 'blender_dna_field'

local p = {}
dna_field_instance = p

function p.dna_field_instance(field, pointerSize)
    local self = {}

    if nil == pointerSize then pointerSize = 4 end
    self.field = field
    self.pointerSize = pointerSize

    return self
end

function p.read(self, data)
    local result = {}
    local num = dna_field.num_array_items(self.field)
    
    for i=1, num do
        if dna_field.is_pointer(self.field) then
            table.insert(result, blender_util.convert(data:read(4)))
            if self.pointerSize > 4 then
                table.insert(result, blender_util.convert(data:read(4)))
            end
			print("simple pointer: " .. result[i])
        elseif dna_field.is_simple_type(self.field) then
            if self.field.type == "void" then
            elseif self.field.type == "char" then
                table.insert(result, data:read(1))
            elseif self.field.type == "short" then
                table.insert(result, blender_util.convert(data:read(2)))
            elseif self.field.type == "int" then
                table.insert(result, blender_util.convert(data:read(4)))
            elseif self.field.type == "float" then
                table.insert(result, blender_util.convertfloat(data:read(4)))
            elseif self.field.type == "double" then
                table.insert(result, blender_util.convert(data:read(8)))
            else
                error("blender> Wrong simple type: " .. self.field.type)
            end
        end
    end

    return result
end

return dna_field_instance

