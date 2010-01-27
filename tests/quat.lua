local function quat_print(n)
    print("Quat: (" .. n.w .. "," .. n.x .. "," .. n.y .. "," .. n.z .. ")")
end

q = quat.new_quat()
r = quat.new_quat()

print(q)
q.x = 10.0
r.y = 1

q = quat.add(q, r)
quat_print(q)

f = quat.dot(q, r)
print(f)

q = quat.from_axis(q)
quat_print(q)

f = quat.length(q)
print(f)

q.x, q.y, q.z, q.w = 5, 4, 3, 2
r.x, r.y, r.z, r.w = 10, 8, 6, 4
s = quat.lerp(q, r, 2)
quat_print(s)

s = quat.mul(q, r)
quat_print(s)

s = quat.normalize(s)
quat_print(s)

s = quat.scale(q, 2)
quat_print(s)

s = quat.slerp(q, r, 2)
quat_print(s)

s = quat.sub(r, q)
quat_print(s)

s = quat.to_axis(q)
quat_print(s)

m = quat.to_mat4(q)
table.foreach(getmetatable(m),print)
print(mat4.mat4_get(m, 0, 0))

