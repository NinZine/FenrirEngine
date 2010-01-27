local function vec3_string(v)
    return "(" .. v.x .. "," .. v.y .. "," .. v.z .. ")"
end

v = vec3.new_vec3()

v.x, v.y, v.z = 1, 2, 3
print("assign: " .. vec3_string(v))

u = vec3.add(v, v)
print("add: " .. vec3_string(u))

f = vec3.dot(v, u)
print("dot: " .. f)

f = vec3.length(v)
print("length: " .. f)

w = vec3.lerp(v, u, 0.5)
print("lerp: " .. vec3_string(w))

w = vec3.mul(u, 2)
print("mul: " .. vec3_string(w))

w = vec3.normalize(w)
print("normalize: " .. vec3_string(w))

w = vec3.project(u, v)
print("project: " .. vec3_string(w))

w = vec3.slerp(v, u, 0.5)
print("slerp: " .. vec3_string(w))

w = vec3.sub(u, v)
print("sub: " .. vec3_string(w))

