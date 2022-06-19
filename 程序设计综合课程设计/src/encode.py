import hashlib
list =[]
list2 = []
with open("InputData.txt", "r+") as f:
    for line in f.readlines():
        line = line.strip("\n")
        list.append(line)
for l in list:
    list2.append(hashlib.md5(l.encode(encoding='utf-8')).hexdigest())
with open("InputDataEncoded.txt","w") as f:
    for l in list2:
        f.write(l+"\n")