#include <stdio.h>
#include <string.h>

//S盒 sList[0]解密 sList[1]加密
unsigned short sList[2][16] = {
        {14, 3, 4,  8, 1, 12, 10, 15, 7, 13, 9, 6,  11, 2, 0, 5},
        {14, 4, 13, 1, 2, 15, 11, 8,  3, 10, 6, 12, 5,  9, 0, 7}
};

unsigned short pList[16] = {
        1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16
};

char bit[16][5] = {"0000", "0001", "0010", "0011",
                   "0100", "0101", "0110", "0111",
                   "1000", "1001", "1010", "1011",
                   "1100", "1101", "1110", "1111"};

int cmp(const char *p1, const char *p2, int k) {
    for (int i = 0; i < k; i++) {
        if (p1[i] != p2[i]) {
            return 1;
        }
    }
    return 0;
}

int hexToInt(const char *p) {
    if (cmp(p, "0000", 4) == 0) return 0;
    else if (cmp(p, "0001", 4) == 0) return 1;
    else if (cmp(p, "0010", 4) == 0) return 2;
    else if (cmp(p, "0011", 4) == 0) return 3;
    else if (cmp(p, "0100", 4) == 0) return 4;
    else if (cmp(p, "0101", 4) == 0) return 5;
    else if (cmp(p, "0110", 4) == 0) return 6;
    else if (cmp(p, "0111", 4) == 0) return 7;
    else if (cmp(p, "1000", 4) == 0) return 8;
    else if (cmp(p, "1001", 4) == 0) return 9;
    else if (cmp(p, "1010", 4) == 0) return 10;
    else if (cmp(p, "1011", 4) == 0) return 11;
    else if (cmp(p, "1100", 4) == 0) return 12;
    else if (cmp(p, "1101", 4) == 0) return 13;
    else if (cmp(p, "1110", 4) == 0) return 14;
    else return 15;
}

char binaryToHex(const char *p) {
    int a = hexToInt(p);
    if (a >= 0 && a <= 9) {
        return (char) (a + '0');
    } else {
        return (char) (a - 10 + 'a');
    }
}

void getKeys(const char key[], char keys[][33]) {
    char binaryStringKey[33];
    for (int i = 0; i < 8; i++) {
        if (key[i] <= '9' && key[i] >= '0') {
            strcpy(binaryStringKey + 4 * i, bit[key[i] - '0']);
        } else {
            strcpy(binaryStringKey + 4 * i, bit[key[i] - 'a' + 10]);
        }
    }
    for (int i = 0; i <= 16; i += 4) {
        memcpy(keys[i / 4], binaryStringKey + i, 16);
    }
}

void getW(const char w[], char W[]) {

    for (int i = 0; i < 4; i++) {
        if (w[i] <= '9' && w[i] >= '0') {
            strcpy(W + 4 * i, bit[w[i] - '0']);
        } else {
            strcpy(W + 4 * i, bit[w[i] - 'a' + 10]);
        }
    }

}


void XOR(const char p1[], const char p2[], char ret[]) {
    for (int i = 0; i < 16; i++) {
        ret[i] = (char) (((p1[i] - '0') ^ (p2[i] - '0')) + '0');
    }
}

void S(const char u[], char v[]) {
    char v1[5];
    for (int i = 0; i < 16; i += 4) {
        memcpy(v1, u + i, 4);
        int j = hexToInt(v1);
        strcpy(v + i, bit[sList[1][j]]);
    }
}

void reversedS(const char u[], char v[]) {
    char v1[5];
    for (int i = 0; i < 16; i += 4) {
        memcpy(v1, u + i, 4);
        int j = hexToInt(v1);
        strcpy(v + i, bit[sList[0][j]]);
    }
}

void P(const char u[], char v[]) {
    for (int i = 0; i < 16; i++) {
        v[i] = u[pList[i] - 1];
    }
}

int main() {
    int n;
    scanf("%d", &n);
    char key[9] = {0};
    char w[5] = {0};
    char res[5] = {0};
    char p[5] = {0};
    char y[17] = {0};
    char keys[5][33] = {0};
    char W[17] = {0};
    char finalV[17] = {0};
    char u[17] = {0};
    char v[17] = {0};
    char hex;
    char finalL[17] = {0};
    char l[17] = {0};
    for (int i = 0; i < n; i++) {
        scanf("%s", key);
        scanf("%s", w);
        getKeys(key, keys);
        getW(w, W);
        for (int j = 0; j < 4; j++) {
            XOR(W, keys[j], u);
            S(u, v);
            P(v, W);
            strcpy(finalV, v);
        }
        XOR(finalV, keys[4], y);

        for (int j = 0; j < 16; j += 4) {
            memcpy(p, y + j, 4);
            hex = binaryToHex(p);
            res[j / 4] = hex;
        }
        printf("%s ", res);

        //将y最后一位取反，获得密文
        if (y[15] == '1') {
            y[15] = '0';
        } else {
            y[15] = '1';
        }
        XOR(y, keys[4], y);

        for (int j = 3; j >= 0; j--) {
            reversedS(y, u);
            XOR(u, keys[j], l);
            strcpy(finalL, l);
            P(l, y);
        }
        for (int j = 0; j < 16; j += 4) {
            memcpy(p, finalL + j, 4);
            hex = binaryToHex(p);
            res[j / 4] = hex;
        }
        printf("%s\n", res);

    }
    return 0;
}