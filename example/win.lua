tt = {
    android = {
        os=1001,
        width=1024,
        height=768
    },
    iOS = {
        os=1002,
        width=874,
        height=980
    }
}


function f(x, y)
    return x*y;
end

for i=1, 10000, 1 do
 f(width, height);
end

