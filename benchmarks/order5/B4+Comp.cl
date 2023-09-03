n = 6;

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
internal[6] compression(internal[6] a, internal[6] b) {
    random r05;
    random r03;
    random r01;
    #i = 1; j = 0 - 3
    random r15;
    random r13;
    #i = 2; j = 0 - 2
    random r25;
    random r23;
    #i = 3; j = 0 - 1
    random r35;
    #i = 4; j = 0 - 0
    random r45;

    #j = 4, 2
    random r4;
    random r2;

    #i = 0, 1, 2, 3, 4, 5
    #i = 0;
    internal c0 = a[0] * b[0];
    #j = 5, 3;
    #j = 5;
    internal t05 = r05 ^ a[0] * b[5] ^ a[5] * b[0] ^ r4 ^ a[0] * b[4] ^ a[4] * b[0];
    #j = 3;
    internal t03 = r03 ^ a[0] * b[3] ^ a[3] * b[0] ^ r2 ^ a[0] * b[2] ^ a[2] * b[0];
    internal t01 = r01 ^ a[0] * b[1] ^ a[1] * b[0];

    #i = 1;
    internal c1 = a[1] * b[1];
    #j = 5, 3;
    #j = 5;
    internal t15 = r15 ^ a[1] * b[5] ^ a[5] * b[1] ^ r4 ^ a[1] * b[4] ^ a[4] * b[1];
    #j = 3;
    internal t13 = r13 ^ a[1] * b[3] ^ a[3] * b[1] ^ r2 ^ a[1] * b[2] ^ a[2] * b[1];
    c1 = c1 ^ r01;

    #i = 2;
    internal c2 = a[2] * b[2];
    #j = 5
    internal t25 = r25 ^ a[2] * b[5] ^ a[5] * b[2] ^ r4 ^ a[2] * b[4] ^ a[4] * b[2];
    internal t23 = r23 ^ a[2] * b[3] ^ a[3] * b[2];

    #i = 3;
    internal c3 = a[3] * b[3];
    #j = 5;
    internal t35 = r35 ^ a[3] * b[5] ^ a[5] * b[3] ^ r4 ^ a[3] * b[4] ^ a[4] * b[3];
    c3 = c3 ^ r23;
    c3 = c3 ^ r13;
    c3 = c3 ^ r03;

    #i = 4;
    internal c4 = a[4] * b[4];
    internal t45 = r45 ^ a[4] * b[5] ^ a[5] * b[4];

    #i = 5;
    internal c5 = a[5] * b[5];
    #j = 4 - 0
    c5 = c5 ^ r45;
    c5 = c5 ^ r35;
    c5 = c5 ^ r25;
    c5 = c5 ^ r15;
    c5 = c5 ^ r05;

    internal[6] res;
    res[0] = c0;
    res[1] = c1;
    res[2] = c2;
    res[3] = c3;
    res[4] = c4;
    res[5] = c5;

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