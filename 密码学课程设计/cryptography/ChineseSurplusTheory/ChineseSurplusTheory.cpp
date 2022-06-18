#include <gmp.h>
#include <stdio.h>
//$g++ test.cpp -o test -lgmp
#pragma GCC optimize("O3")

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
void CRT(mpz_t c,mpz_t d,mpz_t p,mpz_t q){
    mpz_t a;
    mpz_init(a);
    mpz_t b;
    mpz_init(b);
    mpz_powm(a,c,d,p);
    mpz_powm(b,c,d,q);
    mpz_t qInverse;
    mpz_init(qInverse);
    mpz_t pInverse;
    mpz_init(pInverse);
    extendedEuclid(pInverse,p,q);
    extendedEuclid(qInverse,q,p);
    mpz_mul(a,a,q);
    mpz_mul(a,a,qInverse);
    mpz_mul(b,b,p);
    mpz_mul(b,b,pInverse);
    mpz_add(a,a,b);
    mpz_t s;
    mpz_init(s);
    mpz_mul(s,p,q);
    mpz_mod(a,a,s);
    gmp_printf("%Zd\n",a);
}


int main() {
    int n;
    mpz_t e, p, q;
    mpz_init(e);
    mpz_init(p);
    mpz_init(q);
    scanf("%d", &n);
    gmp_scanf("%Zd%Zd%Zd",p,q,e);

    mpz_t d;
    mpz_init(d);
    mpz_t phi;
    mpz_init(phi);
    mpz_t pminus1;
    mpz_t qminus1;
    mpz_init(pminus1);
    mpz_init(qminus1);
    mpz_sub_ui(pminus1, p, 1);
    mpz_sub_ui(qminus1, q, 1);
    mpz_mul(phi, pminus1, qminus1);
    extendedEuclid(d,e,phi);

    for(int i = 0;i < n;i++){
        mpz_t c;
        mpz_init(c);
        gmp_scanf("%Zd",c);
        CRT(c,d,p,q);
    }
    return 0;
}
