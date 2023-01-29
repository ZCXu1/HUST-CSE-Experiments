def power_mod(g, a, p):
    return (g ** a) % p


def is_prime(p):
    for i in range(2, int(p ** 0.5)):
        if p % i == 0:
            return False
    return True


def get_generator(p):
    if p <= 2 or not is_prime(p):
        return 1
    a = 2
    while 1:
        if a ** (p - 1) % p == 1:
            num = 2
            while num < p - 1:
                if a ** num % p == 1:
                    break
                num += 1
            if num == p - 1:
                return a
        a += 1


def primes_InRange(x, y):
    prime_list = []
    for n in range(x, y):
        PrimeFlag = True

        for num in range(2, n):
            if n % num == 0:
                PrimeFlag = False

        if PrimeFlag:
            prime_list.append(n)
    return prime_list


def get_key(cal_A, cal_B, p):
    return (cal_A ** cal_B) % p
