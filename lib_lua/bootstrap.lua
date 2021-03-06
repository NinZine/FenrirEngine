string.split = function(str, pattern)
	pattern = pattern or "[^%s]+"
	if pattern:len() == 0 then pattern = "[^%s]+" end
	local parts = {__index = table.insert}
	setmetatable(parts, parts)
	str:gsub(pattern, parts)
	setmetatable(parts, nil)
	parts.__index = nil
	return parts
end

local p = string.split(package.path, "[^%?]+")
package.path = package.path .. ";" .. p[1] .. "lib_lua/?.lua"

require 'config'
require 'blender/blender'
if config.console == true then
	require 'console'
end
