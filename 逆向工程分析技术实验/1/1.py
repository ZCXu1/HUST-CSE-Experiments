str = '1234 is an exa5678'
ans = ''

for i in range(len(str)):
    t = chr(ord(str[i])^0x2E^0x2D)
    if i == 7:
        t = 'A'
    ans += t
    
print(ans)

# 2107#jpAbm#f{b654;