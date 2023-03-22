STEGO='f4.jpeg';
len=52928;
RES='f4.txt';

f4_simulation(STEGO,len,RES);

function f4_simulation(STEGO,len,RES)
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
        while(abs(DCT(pos(j)))==0)
            j=j+1;
        end
        if(DCT(pos(j))>0)
            fwrite(fp,mod(DCT(pos(j)),2),'ubit1');
        else
            fwrite(fp,mod(DCT(pos(j))+1,2),'ubit1');
        end
        j=j+1;
    end
    fclose(fp);
end