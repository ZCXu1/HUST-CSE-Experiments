
#include <gmp.h>

//ans = base的n次方模mod
void powM(mpz_t ans,mpz_t base,mpz_t n,mpz_t mod){
    mpz_t a;
    mpz_t oe;
    mpz_init(a);
    mpz_init(oe);
    mpz_set_ui(a,1);
    while(mpz_cmp_ui(n,0) > 0){
        mpz_mod_ui(oe,n,2);
        if(mpz_cmp_ui(oe,1) == 0){
            mpz_mul(a,a,base);
            mpz_mod(a,a,mod);
        }
        mpz_pow_ui(base,base,2);
        mpz_mod(base,base,mod);
        mpz_div_ui(n,n,2);
    }
    mpz_set(ans,a);
}

int main(){
    int n;
    scanf("%d",&n);
    for(int i = 0;i < n;i++){
        mpz_t e,m,p,q;
        mpz_init(e);
        mpz_init(m);
        mpz_init(p);
        mpz_init(q);
        gmp_scanf("%Zd%Zd%Zd%Zd",e,m,p,q);
        mpz_t ans;
        mpz_init(ans);
        mpz_t mul;
        mpz_init(mul);
        mpz_mul(mul,p,q);
        powM(ans,m,e,mul);
        gmp_printf("%Zd\n",ans);
    }
    return 0;
}