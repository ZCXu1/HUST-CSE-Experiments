︠f21a074d-c189-4b61-8153-6d4d488d7546s︠
import random
from ngram_score import ngram_score
#参数初始化
ciphertext ='UNGLCKVVPGTLVDKBPNEWNLMGVMTTLTAZXKIMJMBBANTLCMOMVTNAAMILVTMCGTHMKQTLBMVCMXPIAMTLBMVGLTCKAUILEDMGPVLDHGOMIZWNLMGBZLGKSMAZBMKOMKTWNLMGBZKTLCKAMHMIMDMVGBZLXBLCSAZTBMMOMTVPGMOMVKJLTQPXCBPNEJLBBLUILVDKJKZ'
parentkey = list('ABCDEFGHIJKLMNOPQRSTUVWXYZ')
#只是用来声明key是个字典
key = {'A':'A'}
#读取quadgram statistics
fitness = ngram_score('english_quadgrams.txt')
parentscore = -99e9
maxscore = -99e9

print('---------------------------start---------------------------')
j=0
while 1:
    j=j+1
    #随机打乱key中的元素
    random.shuffle(parentkey)
    #将密钥做成字典
    #密文:明文
    for i in range(len(parentkey)):
        key[parentkey[i]] = chr(ord('A')+i)
    #用字典一一映射解密
    decipher = ciphertext
    for i in range(len(decipher)):
        decipher = decipher[:i]+key[decipher[i]]+decipher[i+1:]
    parentscore = fitness.score(decipher)#计算适应度
    #在当前密钥下随机交换两个密钥的元素从而寻找是否有更优的解
    count = 0
    while count < 2000:
        a = random.randint(0,25)
        b = random.randint(0,25)
        #随机交换父密钥中的两个元素生成子密钥，并用其进行解密
        parentkey[a],parentkey[b]= parentkey[b],parentkey[a]
        key[parentkey[a]],key[parentkey[b]] = key[parentkey[b]],key[parentkey[a]]
        decipher = ciphertext
        for i in range(len(decipher)):
            decipher = decipher[:i]+key[decipher[i]]+decipher[i+1:]
        score = fitness.score(decipher)
        #此子密钥代替其对应的父密钥，提高明文适应度
        if score > parentscore:
            parentscore = score
            count=0
        else:
            #还原
            parentkey[a],parentkey[b]=parentkey[b],parentkey[a]
            key[parentkey[a]],key[parentkey[b]]=key[parentkey[b]],key[parentkey[a]]
            count +=1
    #输出该key和明文
    if parentscore > maxscore:
        maxscore = parentscore
        print ('Currrent key: '+''.join(parentkey))
        print ('Iteration total:', j)
        decipher = ciphertext
        for i in range(len(decipher)):
            decipher = decipher[:i]+key[decipher[i]]+decipher[i+1:]
        print ('Plaintext: ', decipher.lower(),maxscore)
        sys.stdout.flush()
︡2918a31b-b751-4bff-8022-2263f7b15f98︡{"stdout":"---------------------------start---------------------------\n"}︡{"stdout":"Currrent key: KHAQMJSBNYUIXGZCFVTLPOWRDE"}︡{"stdout":"\nIteration total: 1\nPlaintext:  kintparrunstryahuizwitenresstscomalefehhcistpeversicceltrsepnsbeadstherpemulcestherntspackltzyenurtybnvelowitenhotnagecoheaveaswitenhoastpacebeleyernhotmhtpgcosheevesruneveraftsdumphuizfthhtkltryafao -1063.0106432474554\n"}︡{"stdout":"Currrent key: MHADLQEZKYOCINPGFTBVWJXRUS"}︡{"stdout":"\nIteration total: 2\nPlaintext:  ynpelittopretdisonguneaptarrerchwimavasscnrelakatrnccametralprbaifresatlawomcaresatperlicymegdapotedbpkamhuneapshepizachsaikairuneapshirelicabamadatpshewselzchrsaakartopakativerfowlsongvesseymetdivih -1040.5253557998738\n"}︡{"stdout":"Currrent key: LHASMJWZKQUGDCPIRVTBNOXYEF"}︡{"stdout":"\nIteration total: 3\nPlaintext:  kulanirrolsarmitouyguaelressaschwipefettcusaneversucceparsenlsbeijsaternewopcesaterlasnickpaymeloramblvephguaelthalidechteiveisguaelthisanicebepemerlthawtandchsteevesroleverifasjowntouyfattakparmifih -1023.1777729356446\n"}︡{"stdout":"Currrent key: KICOMQJDNYSAUGLXFBVTPWHRZE"}︡{"stdout":"\nIteration total: 4\nPlaintext:  minocassuntosharuizvioensettotlypabegerrlitocedestillebostecntweaftorescepubletoresnotcalmbozhenusohwndebyvioenryonakelyreadeatvioenryatocalewebehesnryoprocklytreedetsunedesagotfupcruizgorromboshagay -1022.1021560253188\n"}︡{"stdout":"Currrent key: KUCEMXDHLFSBAVPQWITGNOJYZR"}︡{"stdout":"\nIteration total: 6\nPlaintext:  buticannotsingaloudquietnessismyfarewellmusicevensummerinsectsheapsilenceformesilentiscambridgetonightveryquietlyitakemyleaveasquietlyasicameheregentlyiflickmysleevesnotevenawispofcloudwillibringaway -900.592419222168\n"}︡{"stderr":"Error in lines 11-45\n"}︡{"stderr":"Traceback (most recent call last):\n  File \"/usr/local/sage/local/lib/python3.8/site-packages/smc_sagews/sage_server.py\", line 1230, in execute\n    exec(\n  File \"\", line 19, in <module>\n  File \"/projects/3991ad57-c4ed-4bbb-83f0-5265ff973d1a/assignments/ngram_score.py\", line 26, in score\n    if text[i:i+self.L] in self.ngrams: score += ngrams(text[i:i+self.L])\n  File \"src/cysignals/signals.pyx\", line 320, in cysignals.signals.python_check_interrupt\nKeyboardInterrupt\n"}︡{"done":true}
︠d01b20f0-babb-41f6-b41c-450ee631a488s︠

︡c35d7da3-76e4-4b77-8e52-1e2874a4a1e6︡{"done":true}
︠f4a3b9f3-4a0e-425f-9ec6-c38e4a5b1eefs︠

︡ffaa2a2c-c372-4603-8466-36b366a9f0d5︡{"done":true}









