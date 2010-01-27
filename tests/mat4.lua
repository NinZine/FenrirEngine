m = mat4.new_mat4()

n = mat4.copy(m)
mat4.set(m, 0, 0, 10)
mat4.print(m)
mat4.print(n)

print("identity")
mat4.identity(m)
mat4.print(m)

--n = mat4.inverse(m)
--mat4.print(n)

print("lerp: ")
print(n)
print(m)
mat4.print(n)
mat4.print(m)
n = mat4.lerp(n, m, 0.5)
mat4.print(n)
print(n)
print(m)

t = n
n = mat4.mul(n, m)
mat4.print(n)
if n ~= t then
    print("warning:")
end

f = mat4.determinant(m)
print("determinanst: " .. f)

