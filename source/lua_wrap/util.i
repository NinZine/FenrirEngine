%module util;

%include <stdint.i>
%include <carrays.i>

%array_functions(float, float)
%array_functions(uint16_t, uint16)

%luacode {
function util.to_float(t)
    local len = table.maxn(t)
    local tmp = util.new_float(len)

    for i=1, len do
        util.float_setitem(tmp, i-1, t[i])
    end

    return tmp
end
}

%luacode {
function util.to_uint16(t)
    local len = table.maxn(t)
    local tmp = util.new_uint16(len)

    for i=1, len do
        util.uint16_setitem(tmp, i-1, t[i])
    end

    return tmp
end
}

