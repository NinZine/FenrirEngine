require 'BlendFileHeader'
require 'BHeadStruct'
require 'DNAFieldInstance'
require 'DNAField'
require 'DNAStruct'
require 'DNARepository'

local p = {}
BlendFile = p


function p.BlendFile() 
	return {
        _blockByPointer = {},
        _pointerData = {},
        _readPointers = {},
    }
end

function p.getBlockByPointer(self, pointer)
    return self._blockByPointer[pointer];	
end

function p.getBlocksByDNA(self, dnaIndex)
    local result = {}
    local block
    for i, block in pairs(self.blocks) do
        if block.sdnaIndex == dnaIndex then
            table.insert(result, block)	
        end
    end

    return result
end

function p.read(self, data)

    data:seek("set", 0)
    self.header = BlendFileHeader.BlendFileHeader(data);

    self.blocks = p.readBlocks(self, data);
    self.dna = p.readDNA(self, data);
    self.scenes = p.readType(self, data, "Scene");
end

function p.readType(self, data, type)
    local struct = DNARepository.getStructByType(self.dna, type)
    local blocks = p.getBlocksByDNA(self, struct.index)
    local result = {}

    for i,v in ipairs(blocks) do
        BHeadStruct.print(v)
        table.insert(result, p.readBlock(self, data, v))
    end
    
    return result
end

function p.readBlocks(self, data)
    local block = BHeadStruct.BHeadStruct(data, self.header.ptr_size)
    local result = {}

    while block.code ~= "ENDB" do
        table.insert(result, block)
        self._blockByPointer[block.pointer] = block
        data:seek("cur", block.size) -- jump forward to next block
        block = BHeadStruct.BHeadStruct(data, self.header.ptr_size)
    end

    return result
end

function p.readDNA(self, data)
    local dnaBlock = p.sdnaBlock(self)
    local dna

    if dnaBlock then
        data:seek("set", dnaBlock.position)
        dna = DNARepository.DNARepository(data, self.header)
    end

    return dna
end

function p.readCharArray(data)
    local s = "";
    local c
    
    c = data:read(1)
    while c ~= "\0" do
        s = s .. c
        c = data:read(1)
    end

    return s
end

function p.readPointer(self, data)
    local s = BHeadStruct.convert(data:read(4))
    if self.header.ptr_size > 4 then
        s = s + BHeadStruct.convert(data:read(4))
    end

    return s
end

function p.readBlock(self, data, block)
    local struct = self.dna.structs[block.sdnaIndex]
    local result = {}
    
	print("Struct: " .. self.dna.types[struct.type])
    DNAStruct.print(struct)
    data:seek("set", block.position)
	print("block.count: " .. block.count)
    for i=1, block.count do
        local v = p.readStruct(self, data, struct)
        if block.count > 1 then
            table.insert(result, v)
        else
            result = v
        end
    end
	print("Done" .. self.dna.types[struct.type])

    return result
end

function p.readStruct(self, data, struct)
    local result = {}
    
    --print(indent+dna.types[struct.type])
    
    for i, field in ipairs(struct.fields) do
        p.readField(self, data, field, result)
    end
    
    return result
end

function p.readField(self, data, field, object)
    local shortName = DNAField.shortName(field)
    
	print(field.name .. "(" .. field.type .. ")")
    if DNAField.isPointer(field) then
        local pointer = p.readPointer(self, data)
        
		print("complex pointer: " .. pointer)
        if pointer ~= 0 and not self._readPointers[pointer] then
            self._readPointers[pointer] = 1

            self._pointerData[pointer] =
                p.dereferencePointer(self, data, pointer)
            object[shortName] = self._pointerData[pointer]
        else
            object[shortName] = self._pointerData[pointer]
        end
    elseif DNAField.isCType(field) then
        local instance = DNAFieldInstance.DNAFieldInstance(field,
            self.header.ptr_size)
        local value = DNAFieldInstance.read(instance, data)
        
        if field.type == "char" and shortName == "name"
        and DNAField.isArray(field) then
            value = table.concat(value)
            object[shortName] = value --p.readCharArray(data)	
            print(field.name .. " = " .. object[shortName])
        else
            if value then
                if #value == 1 then
					object[shortName] = value[1]
					print(field.name .. " = " .. object[shortName])
				else object[shortName] = value
                end
            end
        end
    else
        local struct = DNARepository.getStructByType(self.dna, field.type)
        if struct then
            object[shortName] = p.readStruct(self, data, struct)
        else
            data:seek("cur", field.length)
        end
    end
end

function p.dereferencePointer(self, data, pointer)
    local position = data:seek("cur")
    local block = p.getBlockByPointer(self, pointer)
    local result
    
    if block then
        print("Block found " .. pointer)
        result = p.readBlock(self, data, block)
    else
        print("Block not found " .. pointer)
    end
    
    data:seek("set", position)
    
    return result
end

function p.sdnaBlock(self)
    
    for i,block in pairs(self.blocks) do
        if block.code == "DNA1" then
            return block
        end
    end
    
    return nil
end

