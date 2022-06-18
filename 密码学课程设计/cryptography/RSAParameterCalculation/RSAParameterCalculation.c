#include <stdio.h>
#include <gmp.h>

int isP(mpz_t p) {
    int cnt = 0;
    gmp_randstate_t s;
    gmp_randinit_mt(s);
    mpz_t b, t, x, a, n;
    mpz_init(b);
    mpz_init(n);
    mpz_init(a);
    mpz_init(t);
    mpz_init(x);
    mpz_init_set(n, p);
    mpz_init_set(n, p);
    mpz_set(t, n);
    mpz_sub_ui(t, t, 1);
    mpz_mod_ui(b, t, 2);
    while (mpz_cmp_ui(b, 0) == 0) {           //  求解t
        mpz_div_ui(t, t, 2);
        mpz_mod_ui(b, t, 2);
        cnt++;
    }
    mpz_sub_ui(t, n, 1);
    if (cnt == 0) {
        return 0;           // 偶数直接排除
    }
    for (int i = 0; i < 100; i++) {
        mpz_urandomm(a, s, n);              // 随机选择a
        if (mpz_cmp_ui(a, 0) != 0 && mpz_cmp_ui(a, 1) != 0) {
            mpz_powm(x, a, t, n);
            if (mpz_cmp_ui(x, 1) == 0) {           //  是素数
                return 1;
            }
            for (int j = 0; j < cnt; j++) {
                if (mpz_cmp(x, t) == 0) {          // 是素数
                    return 1;
                }
                mpz_powm_ui(x, x, 2, n);
            }
        }
    }
    return 0;           // 不是素数
}

void getGcd(mpz_t gcd,mpz_t x,mpz_t y){
    mpz_t temp, a, b;
    mpz_init(temp);
    mpz_init(a);
    mpz_init(b);
    mpz_init_set(a, x);
    mpz_init_set(b, y);
    mpz_mod(temp, a, b);
    while(mpz_cmp_ui(temp, 0) != 0){
        mpz_init_set(a, b);
        mpz_init_set(b, temp);
        mpz_mod(temp, a, b);
    }
    mpz_set(gcd,b);
}

//求b模m的逆
void extendedEuclid(mpz_t ans,mpz_t b,mpz_t m){
    mpz_t a1,a2,a3,b1,b2,b3,t1,t2,t3;
    mpz_init(a1);
    mpz_init(a2);
    mpz_init(a3);
    mpz_init(b1);
    mpz_init(b2);
    mpz_init(b3);
    mpz_init(t1);
    mpz_init(t2);
    mpz_init(t3);
    mpz_set_ui(a1,1);
    mpz_set_ui(a2,0);
    mpz_set(a3,m);
    mpz_set_ui(b1,0);
    mpz_set_ui(b2,1);
    mpz_set(b3,b);
    while (1){
        if(mpz_cmp_ui(b3,0) == 0){
            return;
        }else if(mpz_cmp_ui(b3,1) == 0){
            if(mpz_cmp_ui(b2,0) < 0){
                mpz_add(b2,b2,m);
            }
            mpz_set(ans,b2);
            return;
        }else{
            mpz_t q;
            mpz_init(q);
            mpz_div(q,a3,b3);
            mpz_t v1,v2,v3;
            mpz_init(v1);
            mpz_init(v2);
            mpz_init(v3);
            mpz_mul(v1,q,b1);
            mpz_mul(v2,q,b2);
            mpz_mul(v3,q,b3);
            mpz_sub(t1,a1,v1);
            mpz_sub(t1,a1,v1);
            mpz_sub(t2,a2,v2);
            mpz_sub(t3,a3,v3);
            mpz_set(a1,b1);
            mpz_set(a2,b2);
            mpz_set(a3,b3);
            mpz_set(b1,t1);
            mpz_set(b2,t2);
            mpz_set(b3,t3);
        }
    }
}

void calculateD(mpz_t e, mpz_t p, mpz_t q) {
    mpz_t phi, x, y, gcd;
    mpz_init(phi);
    mpz_init(x);
    mpz_init(y);
    mpz_init(gcd);
    mpz_sub(x, p, q);
    mpz_div_ui(y, p, 10);
    mpz_abs(x, x);
    if (mpz_cmp_ui(e, 10) < 0) {              // e过小
        printf("ERROR\n");
        return;
    }
    if (!(isP(p) && isP(q))) {
        printf("ERROR\n");
        return;
    }
    if (mpz_cmp(x, y) < 0) {              // p,q差值过小
        printf("ERROR\n");
        return;
    }
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    getGcd(gcd, p, q);
    if (mpz_cmp_ui(gcd, 100000) > 0) {
        printf("ERROR\n");
        return;
    }
    mpz_mul(phi, p, q);
    mpz_t d;
    mpz_init(d);
    getGcd(gcd, e, phi);
    if (mpz_cmp_ui(gcd, 1) != 0) {
        printf("ERROR\n");
    } else {
        extendedEuclid(d,e,phi);
        gmp_printf("%Zd\n", d);
    }
}

int main() {
    int n;
    mpz_t e, p, q;
    mpz_init(e);
    mpz_init(p);
    mpz_init(q);
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        gmp_scanf("%Zd%Zd%Zd", e, p, q);
        calculateD(e, p, q);
    }
    return 0;
}