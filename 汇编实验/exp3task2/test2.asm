;matchº¯Êý
.386
.model flat,c
public match
.code
match proc uses esi edi,s1:ptr,s2:ptr
	mov esi,s1
	mov edi,s2
	cld
	mov ecx,10
	repz cmpsb
	jne False
	mov eax,1
	ret
False: mov eax,0
	ret
match endp
end