%module util

%include <stdint.i>
%include <carrays.i>

%array_functions(float, float)

%luacode {
function util.to_float(t)
    local len = table.maxn(t)
    local tmp = util.new_float(len)

    for i=1, len do
        util.float_setitem(tmp, i-1, t[i])
    end

    return tmp
end
%}



