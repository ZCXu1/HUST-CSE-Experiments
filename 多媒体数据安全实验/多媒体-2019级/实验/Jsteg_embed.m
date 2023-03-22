COVER='test2.jpeg';
STEGO='jsteg.jpeg';
MSG='1.txt';

Jsteg_simulation(COVER,STEGO,MSG);

function Jsteg_simulation(COVER,STEGO,MSG)
    fp=fopen(MSG,'r');
    [msg,len]=fread(fp,'ubit1');
    msg=msg';
%     msg是信息的二进制，len是信息长度
    tic;
    try
        jobj=jpeg_read(COVER);
        DCT=jobj.coef_arrays{1};
        DCT2=DCT;
%         得到DCT
    catch
        error('ERROR(problem with the cover image)');
    end

    AC=numel(DCT)-numel(DCT(1:8:end,1:8:end));
%     AC系数的个数

    if(length(msg)>AC)
        error('ERROR(too long message)');
    end
    pos=true(size(DCT));
    pos(1:8:end,1:8:end)=false;
    pos=find(pos);
%     DCT中非0元素的位置

    j=1;
    for i=1:len
%         找abs>1的AC系数
        while(abs(DCT(pos(j)))<=1)
            j=j+1;
            if(j>=AC)
                break;
            end
        end
%         嵌入第pos(j)个AC系数
        if(j>=AC)
            break;
        end
        if(DCT(pos(j))>1)
            DCT2(pos(j))=DCT2(pos(j))-mod(DCT2(pos(j)),2)+msg(i);
        else
            DCT2(pos(j))=DCT2(pos(j))-mod(DCT2(pos(j))+1,2)+msg(i);
        end
        j=j+1;
        if(j>=AC)
            break;
        end
    end 
%     直方图
%     figure();
%     subplot(121);histogram(DCT);axis([-10,10,0,7*1e4]);title("嵌入前");
%     subplot(122);histogram(DCT2);axis([-10,10,0,7*1e4]);title("嵌入后");
%     值对现象
%     figure();
%     subplot(121);histogram(DCT,(10:1:30));title("嵌入前");
%     subplot(122);histogram(DCT2,(10:1:30));title("嵌入后");
    try
        jobj.coef_arrays{1}=DCT2;
        jobj.optimize_coding=1;
        jpeg_write(jobj,STEGO);
    catch
        error('ERROR(problem with saving the stego image)')
    end
    fclose(fp);
    T=toc;
    fprintf('嵌入时间:%4f秒\n',T);
    fprintf('嵌入长度:%d\n',len);
    
%     图片对比
%     figure();
%     subplot(121);imshow(COVER);title("嵌入前");
%     subplot(122);imshow(STEGO);title("嵌入后");
end
