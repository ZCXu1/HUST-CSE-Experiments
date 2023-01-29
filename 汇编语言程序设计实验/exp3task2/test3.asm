;continueOrQuitº¯Êý
.386
.model flat,c
public continueOrQuit
.code
continueOrQuit proc uses esi,s1:ptr
	mov esi,s1
	cld
	lodsw
	cmp ax,0000h
	je Continue
	cmp ax,0071h
	je Quit
	cmp ax,061h
	je CalAve
	mov eax,0
	ret
Continue: mov eax,1
		ret
Quit:mov eax,2
		ret
CalAve:mov eax,3
		ret
continueOrQuit endp
end
