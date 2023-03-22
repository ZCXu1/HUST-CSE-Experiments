COVER='test2.jpeg';
STEGO='f4.jpeg';
MSG='1.txt';
f4_simulation(COVER,STEGO,MSG);

function f4_simulation(COVER,STEGO,MSG)
    fp=fopen(MSG,'r');
    [msg,len]=fread(fp,'ubit1');
    msg=msg';
%     计时、计数
    tic;
    sjy=0;
    try
        jobj=jpeg_read(COVER);
        DCT=jobj.coef_arrays{1};
        DCT2=DCT;
    catch
        error('ERROR(problem with the cover image)');
    end

    AC=numel(DCT)-numel(DCT(1:8:end,1:8:end));

    if(length(msg)>AC)
        error('ERROR(too long message)');
    end
    
    pos=true(size(DCT));
    pos(1:8:end,1:8:end)=false;
    pos=find(pos);

    j=1;
    for i=1:len
        while(1)
		%     寻找可以嵌入的AC系数
            if((DCT(pos(j))==1 && msg(i)==0) || (DCT(pos(j))==-1 && msg(i)==1) || (DCT(pos(j))==0))
                if(DCT(pos(j))~=0)
                    sjy=sjy+1;
                end
                DCT2(pos(j))=0;
                j=j+1;           
                if(j>=AC)
                    break;
                end
            else
                break;
            end
        end
%         嵌入第pos(j)个AC系数
        if(j>=AC)
            break;
        end
        if(DCT(pos(j))>0 && msg(i)~=mod(DCT2(pos(j)),2))
            sjy=sjy+1;
            DCT2(pos(j))=DCT2(pos(j))-1;
        elseif(DCT(pos(j))<0 && msg(i)~=mod(DCT2(pos(j))+1,2))
            sjy=sjy+1;
            DCT2(pos(j))=DCT2(pos(j))+1;
        end
        j=j+1;
        if(j>=AC)
            break;
        end
    end
%     直方图
%     figure();
%     subplot(121);histogram(DCT);axis([-10,10,0,7*1e4]);title("f4嵌入前");
%     subplot(122);histogram(DCT2);axis([-10,10,0,7*1e4]);title("f4嵌入后");
%     无值对现象
%     figure();
%     subplot(121);histogram(DCT,(10:1:30));title("f4嵌入前");
%     subplot(122);histogram(DCT2,(10:1:30));title("f4嵌入后");
    try
        jobj.coef_arrays{1}=DCT2;
        jobj.optimize_coding=1;
        jpeg_write(jobj,STEGO);
    catch
        error('ERROR(problem with saving the stego image)')
    end
%     图片对比
%     figure();
%     subplot(121);imshow(COVER);title("嵌入前");
%     subplot(122);imshow(STEGO);title("嵌入后");
    T=toc;
    fprintf('嵌入时间:%4f秒\n',T);
    fprintf('嵌入信息:%d比特\n',i);
    fprintf('修改的AC系数个数:%d个\n',sjy);
    fclose(fp);
end