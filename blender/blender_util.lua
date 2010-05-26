local p = {}
blender_util = p

function p.convert(str)
  local function _b2n(exp, num, digit, ...)
    if not digit then return num end
    return _b2n(exp*256, num + digit*exp, ...)
  end
  return _b2n(256, string.byte(str, 1, -1))
end

function p.convertfloat(str)
      -- Change to b4,b3,b2,b1 to unpack an LSB float
      local b4,b3,b2,b1 = string.byte(str, 1, 4)
      local exponent = (b1 % 128) * 2 + math.floor(b2 / 128)
      if exponent == 0 then return 0 end
      local sign = (b1 > 127) and -1 or 1
      local mantissa = ((b2 % 128) * 256 + b3) * 256 + b4
      mantissa = (math.ldexp(mantissa, -23) + 1) * sign
      return math.ldexp(mantissa, exponent - 127)
end

function p.quad_to_tri(face, vertex)
	
end


return blender_util

