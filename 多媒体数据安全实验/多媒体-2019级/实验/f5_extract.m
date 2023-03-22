STEGO='f5.jpeg';
len=52928;
RES='f5.txt';
f5_simulation(STEGO,len,RES);

function f5_simulation(STEGO,len,RES)
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
    a=[0,0,0];
    i=1;
    j=1;
    k=1;
    while(i<=len)
        while(1)
            if(DCT(pos(j))==0)
                j=j+1;
            else
                if(DCT(pos(j))>0)
                    a(k)=mod(DCT(pos(j)),2);
                else
                    a(k)=mod(DCT(pos(j))+1,2);
                end
                k=k+1;
                j=j+1;
                if(k==4)
                    k=k-3;
                    break;
                end
            end
        end
        fwrite(fp,xor(a(1),a(2)),'ubit1');
        fwrite(fp,xor(a(3),a(2)),'ubit1');
        i=i+2;
    end
        fclose(fp);
    end
    