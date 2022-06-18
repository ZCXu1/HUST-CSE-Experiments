#include<iostream>

using namespace std;

unsigned short SList[2][16] = {
        {14, 3, 4,  8, 1, 12, 10, 15, 7, 13, 9, 6,  11, 2, 0, 5},
        {14, 4, 13, 1, 2, 15, 11, 8,  3, 10, 6, 12, 5,  9, 0, 7}
};


/*
 * 结构体中常见的冒号的用法是表示位域。
 有些信息在存储时，并不需要占用一个完整的字节，而只需占几个或一个二进制位。
 所谓“位域”是把一个字节中的二进位划分为几个不同的区域，
 并说明每个区域的位数。每个域有一个域名，允许在程序中按域名进行操作。
 这样就可以把几个不同的对象用一个字节的二进制位域来表示。
 * */
struct bit4 {
    unsigned short
            key4 : 4, key3 : 4, key2 : 4, key1 : 4;
};

struct b16 {

    unsigned short
            key16 : 1, key15 : 1, key14 : 1, key13 : 1,
            key12 : 1, key11 : 1, key10 : 1, key9 : 1,
            key8 : 1, key7 : 1, key6 : 1, key5 : 1,
            key4 : 1, key3 : 1, key2 : 1, key1 : 1;
};

/*
 * 结构体和共用体的区别在于：结构体的各个成员会占用不同的内存，互相之间没有影响；
 * 而共用体的所有成员占用同一段内存，修改一个成员会影响其余所有成员。
 * 结构体占用的内存大于等于所有成员占用的内存的总和（成员之间可能会存在缝隙），
 * 共用体占用的内存等于最长的成员占用的内存。共用体使用了内存覆盖技术，
 * 同一时刻只能保存一个成员的值，如果对新的成员赋值，就会把原来成员的值覆盖掉。
 * */
union unibit {
    unsigned short num;
    bit4 b4;
    b16 b16;
};

// 内联函数可以解决一些频繁调用的函数大量消耗栈空间（栈内存）的问题。
// inline 加速读入，c表示读取字符，res表示返回值。
inline unsigned short read() {
    char c = getchar();
    return ((c >= 'a') ? (c - 'a' + 10) : (c - '0'));
}

void S(int mod, unibit &t) {
    t.b4.key1 = SList[mod][t.b4.key1];
    t.b4.key2 = SList[mod][t.b4.key2];
    t.b4.key3 = SList[mod][t.b4.key3];
    t.b4.key4 = SList[mod][t.b4.key4];
}

inline void print(unsigned short x) {
    for (int i = 1; i <= 4; i++) {
        int c = (x >> (4 * (4 - i))) & 0xF;
        if (c >= 0 && c <= 9)
            putchar('0' + c);
        else if (c >= 10 && c <= 15)
            putchar('a' + c - 10);
    }
}


void P(unibit &p) {
    unibit r = p;
    p.b16.key1 = r.b16.key1;
    p.b16.key2 = r.b16.key5;
    p.b16.key3 = r.b16.key9;
    p.b16.key4 = r.b16.key13;
    p.b16.key5 = r.b16.key2;
    p.b16.key6 = r.b16.key6;
    p.b16.key7 = r.b16.key10;
    p.b16.key8 = r.b16.key14;
    p.b16.key9 = r.b16.key3;
    p.b16.key10 = r.b16.key7;
    p.b16.key11 = r.b16.key11;
    p.b16.key12 = r.b16.key15;
    p.b16.key13 = r.b16.key4;
    p.b16.key14 = r.b16.key8;
    p.b16.key15 = r.b16.key12;
    p.b16.key16 = r.b16.key16;
}


// mod == 1 代表加密， 0代表解密
void SPN(unsigned short mod, unibit key[6], unibit &t) {
    unsigned short ans = 0;
    if (mod) {
        for (int i = 1; i <= 4; i++) {
            t.num ^= key[i].num;
            S(1, t);
            ans = t.num;
            P(t);
        }
        ans ^= key[5].num;
        t.num = ans;
    } else {
        t.num ^= key[5].num;
        for (int i = 4; i >= 1; i--) {
            S(0, t);
            ans = t.num;
            t.num ^= key[i].num;
            P(t);
        }
        ans ^= key[1].num;
    }
    print(ans);
}

int main() {
    int n;
    unibit K[6], t{};
    unsigned short tmp[9];
    scanf("%d", &n);
    getchar();
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= 8; ++j) {
            // key 各个bit值
            tmp[j] = read();
        }
        for (int j = 1; j <= 5; ++j) {
            K[j].b4.key1 = tmp[j + 0];
            K[j].b4.key2 = tmp[j + 1];
            K[j].b4.key3 = tmp[j + 2];
            K[j].b4.key4 = tmp[j + 3];
        }
        getchar();
        t.b4.key1 = read();
        t.b4.key2 = read();
        t.b4.key3 = read();
        t.b4.key4 = read();
        getchar();
        SPN(1, K, t);
        putchar(' ');
        t.b16.key16 ^= 1;
        SPN(0, K, t);
        putchar('\n');
    }

    return 0;
}