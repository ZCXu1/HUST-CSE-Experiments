STEGO='jsteg.jpeg';
RES='Jsteg.txt';
len=52928;

Jsteg_simulation(STEGO,len,RES);

function Jsteg_simulation(STEGO,len,RES)
    fp=fopen(RES,'a');
    try
        jobj=jpeg_read(STEGO);
        DCT=jobj.coef_arrays{1};
    catch
        error('ERROR(problen with the STEGO imag)');
    end

    pos=true(size(DCT));
    pos(1:8:end,1:8:end)=false;
    pos=find(pos);
    j=1;
    for i=1:len
        while(abs(DCT(pos(j)))<=1)
            j=j+1;
        end
        if(DCT(pos(j))>0)
            fwrite(fp,mod(DCT(pos(j)),2),'ubit1');
        elseif(DCT(pos(j))<0)
            fwrite(fp,mod(DCT(pos(j))+1,2),'ubit1');
        end
        j=j+1;
    end
    fclose(fp);
end