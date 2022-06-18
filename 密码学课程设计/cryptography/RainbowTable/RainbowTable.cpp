#include<stdio.h>
#include<string.h>
#include <map>

#pragma GCC optimize("O3")
unsigned int SHA1_tmp;
#define ModN 2176782336
#define SHA1_ROTL(a, b) (SHA1_tmp=(a),((SHA1_tmp>>(32-b))&(0x7fffffff>>(31-b)))|(SHA1_tmp<<b))
#define SHA1_F(B, C, D, t) ((t<40)?((t<20)?((B&C)|((~B)&D)):(B^C^D)):((t<60)?((B&C)|(B&D)|(C&D)):(B^C^D)))

using namespace std;

char beginString[10000][9] = {0}, endString[10000][9] = {0};
int m;
//map2用来保存每一条链的末尾的字符串与链下标的对应关系
//map1用来保存每一条链的开始的字符串与链下标的对应关系
map<string, int> map1, map2;


inline void H(unsigned n, char str[8]) {
    str[0] = 'a';
    str[1] = '0';
    str[2] = '0';
    str[3] = '0';
    str[4] = '0';
    str[5] = '0';
    str[6] = '0';
    str[7] = '0';
    int i = 2;
    while (n) {
        unsigned tmp = n % 36;
        if (tmp < 10)
            str[i++] = tmp + '0';
        else {
            str[i++] = tmp - 10 + 'a';
        }
        n = n / 36;
    }
}


inline void R(unsigned sha1[5], char str[8], int i) {
    H((sha1[0] + sha1[1] * i) % ModN, str);
}


inline int SHA1(const char *str, int length, unsigned sha1[5]) {
    /*
    计算字符串SHA-1
    参数说明：
    str         字符串指针
    length      字符串长度
    sha1         用于保存SHA-1的字符串指针
    返回值为参数sha1
    */
    unsigned char *pp, *ppend;
    unsigned int l, i, K[80], W[80], TEMP, A, B, C, D, E, H0, H1, H2, H3, H4;
    H0 = 0x67452301, H1 = 0xEFCDAB89, H2 = 0x98BADCFE, H3 = 0x10325476, H4 = 0xC3D2E1F0;
    for (i = 0; i < 20; K[i++] = 0x5A827999);
    for (i = 20; i < 40; K[i++] = 0x6ED9EBA1);
    for (i = 40; i < 60; K[i++] = 0x8F1BBCDC);
    for (i = 60; i < 80; K[i++] = 0xCA62C1D6);
    l = length + ((length % 64 > 56) ? (128 - length % 64) : (64 - length % 64));
    if (!(pp = (unsigned char *) malloc((unsigned int) l))) return -1;
    for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++);
    for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; pp[i + 3 - 2 * (i % 4)] = 0, i++);
    *((unsigned int *) (pp + l - 4)) = length << 3;
    *((unsigned int *) (pp + l - 8)) = length >> 29;
    for (ppend = pp + l; pp < ppend; pp += 64) {
        for (i = 0; i < 16; W[i] = ((unsigned int *) pp)[i], i++);
        for (i = 16; i < 80; W[i] = SHA1_ROTL((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]), 1), i++);
        A = H0, B = H1, C = H2, D = H3, E = H4;
        for (i = 0; i < 80; i++) {
            TEMP = SHA1_ROTL(A, 5) + SHA1_F(B, C, D, i) + E + W[i] + K[i];
            E = D, D = C, C = SHA1_ROTL(B, 30), B = A, A = TEMP;
        }
        H0 += A, H1 += B, H2 += C, H3 += D, H4 += E;
    }
    free(pp - l);
    sha1[0] = H0, sha1[1] = H1, sha1[2] = H2, sha1[3] = H3, sha1[4] = H4;
    return 0;
}

inline bool find(char str[9], unsigned sha1[5], int index) {
    int l = 0;
    unsigned temporarySHA1[5];
    char temporaryString[9];
    strcpy(temporaryString,beginString[index]);
    for (int j = 0; j < 10000; j++) {
        SHA1(temporaryString, 8, temporarySHA1);        // 得到sha1
        for (l = 0; l < 5; l++) {
            if (temporarySHA1[l] != sha1[l]) {
                break;
            }
        }
        if (l == 5) {
            strcpy(str, temporaryString);
            return true;
        }
        R(temporarySHA1, temporaryString, j % 100 + 1);    // 得到str
    }

    return false;
}


int main() {
    bool found = false;
    unsigned sha1[5], temporarySHA1[5];
    char str[9] = {0};
    scanf("%d", &m);
    for (int i = 0; i < m; i++) {
        scanf("%s%s", beginString[i], endString[i]);
        string begin = string(beginString[i]);
        string end = string(endString[i]);
        map1.insert(pair<string, int>(begin, i));
        map2.insert(pair<string, int>(end, i));
    }
    scanf("%8x%8x%8x%8x%8x", &(sha1[0]), &(sha1[1]), &(sha1[2]), &(sha1[3]), &(sha1[4]));
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 5; j++)
            temporarySHA1[j] = sha1[j];
        for (int j = 0; j < 10000; j++) {
            R(temporarySHA1, str, (i + j) % 100 + 1);    // 得到str
            if (map2.find(str) != map2.end()) {
                int index = map2[str];
                found = find(str, sha1, index);
                if (found)
                    break;
            }
            SHA1(str, 8, temporarySHA1);        // 得到sha1
        }
        if (found)
            break;
    }
    if (found) {
        printf("%s\n", str);
    } else {
        printf("None\n");
    }
    return 0;
}
