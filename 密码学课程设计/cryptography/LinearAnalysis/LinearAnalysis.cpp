#include<stdio.h>
#include<algorithm>

#pragma GCC optimize("O3")
using namespace std;

#define getc() (_b1==_b2?fread(_b,1,100000,stdin),_b2=_b+100000,*((_b1=_b)++):*(_b1++))
char _b[100000], *_b1, *_b2;

// 0加密 1解密
short int sList[2][16] = {{14, 4, 13, 1, 2, 15, 11, 8,  3, 10, 6, 12, 5,  9, 0, 7},
                          {14, 3, 4,  8, 1, 12, 10, 15, 7, 13, 9, 6,  11, 2, 0, 5}};

//s盒代换 0加密 1解密
unsigned short substitution(int mode, unsigned short text) {

    return (sList[mode][(text & 0xF000) >> 12] << 12) +
           (sList[mode][(text & 0x0F00) >> 8] << 8) +
           (sList[mode][(text & 0x00F0) >> 4] << 4) +
           (sList[mode][(text & 0x000F)]);

};

//p盒代换 用位操作加速
unsigned short permutation(unsigned short text) {
    return (text & 0x8000) +
           ((text & 0x0800) << 3) +
           ((text & 0x0080) << 6) +
           ((text & 0x0008) << 9) +
           ((text & 0x4000) >> 3) +
           ((text & 0x0400)) +
           ((text & 0x0040) << 3) +
           ((text & 0x0004) << 6) +
           ((text & 0x2000) >> 6) +
           ((text & 0x0200) >> 3) +
           ((text & 0x0020)) +
           ((text & 0x0002) << 3) +
           ((text & 0x1000) >> 9) +
           ((text & 0x0100) >> 6) +
           ((text & 0x0010) >> 3) +
           (text & 0x0001);
}

unsigned int SPN(unsigned int key, unsigned short plaintext, unsigned short ciphertext, int mode) {
    unsigned short keys[5];
    keys[0] = ((key & 0xFFFF0000) >> 16);
    keys[1] = ((key & 0x0FFFF000) >> 12);
    keys[2] = ((key & 0x00FFFF00) >> 8);
    keys[3] = ((key & 0x000FFFF0) >> 4);
    keys[4] = (key & 0x0000FFFF);

    unsigned short text = plaintext;

    for (int i = 0; i <= 2; ++i) {
        text ^= keys[i];
        text = substitution(mode, text);
        text = permutation(text);
    }
    text ^= keys[3];
    text = substitution(mode, text);
    text ^= keys[4];

    return (text == ciphertext);
}


// 快读
inline unsigned short read() {
    unsigned short x = 0;
    char c = getc();//x代表返回值，c代表读取的字符
    while (c != ' ' && c != '\n') {
        x = x * 16 + ((c >= 'a') ? (c - 'a' + 10) : (c - '0'));
        c = getc();
    }
    return x;
}

// 快写
inline void write(register unsigned int key) {
    putchar((((key >> 28) & 0x0000000F) >= 10) ? (((key >> 28) & 0x0000000F) - 10 + 'a') : (((key >> 28) & 0x0000000F) +
                                                                                            '0'));
    putchar((((key >> 24) & 0x0000000F) >= 10) ? (((key >> 24) & 0x0000000F) - 10 + 'a') : (((key >> 24) & 0x0000000F) +
                                                                                            '0'));
    putchar((((key >> 20) & 0x0000000F) >= 10) ? (((key >> 20) & 0x0000000F) - 10 + 'a') : (((key >> 20) & 0x0000000F) +
                                                                                            '0'));
    putchar((((key >> 16) & 0x0000000F) >= 10) ? (((key >> 16) & 0x0000000F) - 10 + 'a') : (((key >> 16) & 0x0000000F) +
                                                                                            '0'));
    putchar((((key >> 12) & 0x0000000F) >= 10) ? (((key >> 12) & 0x0000000F) - 10 + 'a') : (((key >> 12) & 0x0000000F) +
                                                                                            '0'));
    putchar((((key >> 8) & 0x0000000F) >= 10) ? (((key >> 8) & 0x0000000F) - 10 + 'a') : (((key >> 8) & 0x0000000F) +
                                                                                          '0'));
    putchar((((key >> 4) & 0x0000000F) >= 10) ? (((key >> 4) & 0x0000000F) - 10 + 'a') : (((key >> 4) & 0x0000000F) +
                                                                                          '0'));
    putchar((((key >> 0) & 0x0000000F) >= 10) ? (((key >> 0) & 0x0000000F) - 10 + 'a') : (((key >> 0) & 0x0000000F) +
                                                                                          '0'));
}

int main() {
    int n, times, maxNum[2];
    pair<int, int> count1[256], count2[256];
    unsigned plaintext[8000], ciphertext[8000], temp[4], k[8], lastKey2, key;
    scanf("%d", &n);
    getchar();
    for (int i = 0; i < n; i++) {
        lastKey2 = -1;
        fill(count1, count1 + 256, pair<int, int>(-4000, 0));
        for (int j = 0; j < 8000; j++) {
            plaintext[j] = read();
            ciphertext[j] = read();
        }
        // 计算第一条链
        for (int j = 0; j < 8000; j++) {
            for (k[5] = 0; k[5] < 16; k[5]++) {
                for (k[7] = 0; k[7] < 16; k[7]++) {
                    temp[1] = ((ciphertext[j] & 0xf00) >> 8) ^ k[5];
                    temp[3] = ((ciphertext[j] & 0xf) ^ k[7]);
                    temp[1] = sList[1][temp[1]];
                    temp[3] = sList[1][temp[3]];
                    if ((((plaintext[j] & 0x800) >> 11) ^ ((plaintext[j] & 0x200) >> 9) ^
                         ((plaintext[j] & 0x100) >> 8) ^
                         (temp[1] & 0x1) ^ ((temp[1] & 0x4) >> 2) ^ (temp[3] & 0x1) ^ ((temp[3] & 0x4) >> 2)) == 0)
                        count1[k[5] * 16 + k[7]].first++;
                }
            }
        }
        // 得到部分密钥
        for (int j = 0; j < 256; j++) {
            count1[j].second = j;
            count1[j].first = abs(count1[j].first);
        }
        sort(count1, count1 + 256);

        for (int j = 255; j >= 0; j--) {
            lastKey2 = k[5];
            k[5] = count1[j].second / 16;
            k[7] = count1[j].second % 16;

            // 计算第二条链
            if (j == 255 || lastKey2 != k[5]) {
                fill(count2, count2 + 256, pair<int, int>(-4000, 0));
                for (int j = 0; j < 8000; j++) {
                    for (k[4] = 0; k[4] < 16; k[4]++) {
                        for (k[6] = 0; k[6] < 16; k[6]++) {
                            temp[0] = ((ciphertext[j] & 0xf000) >> 12) ^ k[4];
                            temp[1] = ((ciphertext[j] & 0xf00) >> 8) ^ k[5];
                            temp[2] = ((ciphertext[j] & 0xf0) >> 4) ^ k[6];
                            temp[0] = sList[1][temp[0]];
                            temp[1] = sList[1][temp[1]];
                            temp[2] = sList[1][temp[2]];
                            if ((((plaintext[j] & 0x800) >> 11) ^ ((plaintext[j] & 0x400) >> 10) ^
                                 ((temp[0] & 0x4) >> 2) ^ (temp[0] & 0x1) ^ ((temp[1] & 0x4) >> 2) ^ (temp[1] & 0x1) ^
                                 ((temp[2] & 0x4) >> 2) ^ (temp[2] & 0x1)) == 0)
                                count2[k[4] * 16 + k[6]].first++;
                        }
                    }
                }
                for (int j = 0; j < 256; j++) {
                    count2[j].second = j;
                    count2[j].first = abs(count2[j].first);
                }
                maxNum[0] = max_element(count2, count2 + 256) - count2;
                count2[maxNum[0]].first = 0;
                maxNum[1] = (*max_element(count2, count2 + 256)).second;
            }

            for (int h = 0; h < 1; h++) {
                k[4] = maxNum[h] / 16;
                k[6] = maxNum[h] % 16;
                for (k[0] = 0; k[0] < 65535; k[0]++) {
                    key = (k[0] << 16) | (k[4] << 12) | (k[5] << 8) | (k[6] << 4) | k[7];
                    for (times = 0; times < 2; times++) {
                        if (!SPN(key, plaintext[times], ciphertext[times], 0))
                            break;
                    }
                    if (times == 2) {
                        write(key);
                        putchar('\n');
                        break;
                    }
                }
                if (times == 2)
                    break;
            }
            if (times == 2)
                break;
        }
    }
    return 0;
}

