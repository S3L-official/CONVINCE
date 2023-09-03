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