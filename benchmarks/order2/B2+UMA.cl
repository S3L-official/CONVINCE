#Tight Private Circuits: Achieving Probing Security with the Least Refreshing
# Fig. 1


n = 3;

share[n] preshare(private k) {
	share[n] x;
	x[n - 1] = k;
    for (i = 0; i <= n - 2; i = i + 1){
        random r;
        x[i] = r;
        x[n - 1] = x[n - 1] ^ r;
    }
    return x;
}

internal[n] XOR(internal[n] a, internal[n] b) {
	internal[n] res;
	int i;
  	for (i = 0; i <= n - 1; i = i + 1) {
  		res[i] = a[i] ^ b[i];
  	}
  	return res;
}

internal[n] SecMult(share[n] a, share[n] b) {
	internal[n] c;
	int i;
    for(i = 0; i <= n - 1; i = i + 1) {
           c[i] = a[i] * b[i];
    }

    int j;
    for(i = 0; i <= n - 1; i = i + 1) {
        for(j = i + 1; j <= n - 1; j = j + 1) {
            random s;
            internal t1 = a[i] * b[j];
            internal t2 = s ^ t1;
            internal t3 = a[j] * b[i];
            internal ss = t2 ^ t3;
            c[i] = c[i] ^ s;
            c[j] = c[j] ^ ss;
        }
    }

	return c;
}

internal[3] compression(internal[3] a, internal[3] b) {
    random r0;
    random r1;
    internal t1 = a[0] * b[0];
    internal t2 = t1 ^ r0;
    internal t3 = a[0] * b[2];
    internal t4 = t2 ^ t3;
    internal t5 = a[2] * b[0];
    internal t6 = t4 ^ t5;

    internal t7 = a[1] * b[1];
    internal t8 = t7 ^ r1;
    internal t9 = a[0] * b[1];
    internal t10 = t8 ^ t9;
    internal t11 = a[1] * b[0];
    internal t12 = t10 ^ t11;

    internal t13 = a[2] * b[2];
    internal t14 = t13 ^ r0;
    internal t15 = t14 ^ r1;
    internal t16 = a[1] * b[2];
    internal t17 = t15 ^ t16;
    internal t18 = a[2] * b[1];
    internal t19 = t17 ^ t18;


    internal[3] res;
    res[0] = t6;
    res[1] = t12;
    res[2] = t19;

    return res;
}

internal[n] example(share[n] a, share[n] b) {
    internal[n] z;
    z = XOR(a, b);
    internal[n] c;
    #c = SecMult(z, a);
    c = compression(z, a);
    return c;
}

internal[n] main(private k1, private k2) {
    internal[n] x;
    x = preshare(k1);
    internal[n] y;
    y = preshare(k2);
    internal[n] z;
    z = example(x, y);
    return z;
}