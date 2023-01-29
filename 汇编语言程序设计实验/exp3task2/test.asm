;calculate()º¯Êý
.386
.model flat,c
public calculate
.code

calculate proc uses esi edi,s:ptr

        mov esi, s      
        mov edi, s     
        add edi, 6      
        cld          
        lodsw         
        add ax, ax   
        add ax, ax     
        mov bx, ax    
        lodsw          
        add ax, ax   
        add bx, ax     
        lodsw           
        add ax, bx      
        mov dl, 7
        div dl          
        mov ah, 0       
        stosw           
        ret
calculate endp
end
