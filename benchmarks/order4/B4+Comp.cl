n = 5;

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

share[n] RefreshM(share[n] a) {
	internal[n] c;
	int i;
	for(i = 0; i <= n - 1; i = i + 1) {
		c[i] = a[i];
	}
    int j;
	for(i = 0; i <= n - 1; i = i + 1) {
		for(j = i + 1; j <= n - 1; j = j + 1) {
            random r;
			c[i] = c[i] ^ r;
			c[j] = c[j] ^ r;
		}
	}

	return c;
}

internal[n] XOR(internal[n] a, internal[n] b) {
	internal[n] res;
	int i;
  	for (i = 0; i <= n - 1; i = i + 1) {
  		res[i] = a[i] ^ b[i];
  	}
  	return res;
}
internal[5] compression(internal[5] a, internal[5] b) {
    random r0;
    random r1;
    random r2;
    random r3;
    random r4;
    internal t1 = a[0] * b[0];
    internal t2 = a[0] * b[1];
    internal t3 = a[1] * b[0];
    internal t4 = a[0] * b[2];
    internal t5 = a[2] * b[0];
    internal t6 = a[1] * b[1];
    internal t7 = a[1] * b[2];
    internal t8 = a[2] * b[1];
    internal t9 = a[1] * b[3];
    internal t10 = a[3] * b[1];
    internal t11 = a[2] * b[2];
    internal t12 = a[2] * b[3];
    internal t13 = a[3] * b[2];
    internal t14 = a[2] * b[4];
    internal t15 = a[4] * b[2];
    internal t16 = a[3] * b[3];
    internal t17 = a[3] * b[4];
    internal t18 = a[4] * b[3];
    internal t19 = a[3] * b[0];
    internal t20 = a[0] * b[3];
    internal t21 = a[4] * b[4];
    internal t22 = a[4] * b[0];
    internal t23 = a[0] * b[4];
    internal t24 = a[4] * b[1];
    internal t25 = a[1] * b[4];

    internal t26 = t1 ^ r0 ^ t2 ^ t3 ^ r1 ^ t4 ^ t5;
    internal t27 = t6 ^ r1 ^ t7 ^ t8 ^ r2 ^ t9 ^ t10;
    internal t28 = t11 ^ r2 ^ t12 ^ t13 ^ r3 ^ t14 ^ t15;
    internal t29 = t16 ^ r3 ^ t17 ^ t18 ^ r4 ^ t19 ^ t20;
    internal t30 = t21 ^ r4 ^ t22 ^ t23 ^ r0 ^ t24 ^ t25;

    internal[5] res;
    res[0] = t26;
    res[1] = t27;
    res[2] = t28;
    res[3] = t29;
    res[4] = t30;

    return res;
}


internal[3][n] example(share[3][n] a) {
    internal[3][n] res;
    internal[n] b0;
    b0 = RefreshM(a[1]);
    res[0] = compression(a[0], b0);
    internal[n] b2;
    b2 = XOR(a[0], a[1]);
    internal[n] b4;
    b4 = XOR(a[1], a[2]);
    internal[n] b3;
    res[1] = compression(b2, b4);
    internal[n] b5;
    res[2] = compression(b2, a[2]);
    return res;
}



internal[n] main(private[3] k) {
    internal[3][n] x;
    int i;
    x[0] = preshare(k[0]);
    x[1] = preshare(k[1]);
    x[2] = preshare(k[2]);
    #for(i = 0; i <= 2; i = i + 1) {
    #    x[i] = preshare(k[i]);
    #}
    internal[3][n] res;
    res = example(x);
    return res;
}