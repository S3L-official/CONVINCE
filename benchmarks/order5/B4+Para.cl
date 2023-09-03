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

share[n] Refresh(share[n] a) {
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
internal[6] parallel(internal[6] a, internal[6] b) {
    random r0;
    random r1;
    random r2;
    random r3;
    random r4;
    random r5;
    random r6;
    random r7;
    random r8;
    random r9;
    random r10;
    random r11;
    internal q1 = a[0] * b[0];
    internal q2 = a[1] * b[1];
    internal q3 = a[2] * b[2];
    internal q4 = a[3] * b[3];
    internal q5 = a[4] * b[4];
    internal q6 = a[5] * b[5];

    internal q7 = a[0] * b[5];
    internal q8 = a[1] * b[0];
    internal q9 = a[2] * b[1];
    internal q10 = a[3] * b[2];
    internal q11 = a[4] * b[3];
    internal q12 = a[5] * b[4];

    internal q13 = a[5] * b[0];
    internal q14 = a[0] * b[1];
    internal q15  = a[1] * b[2];
    internal q16  = a[2] * b[3];
    internal q17  = a[3] * b[4];
    internal q18  = a[4] * b[5];

    internal q19 = a[0] * b[4];
    internal q20 = a[1] * b[5];
    internal q21 = a[2] * b[0];
    internal q22 = a[3] * b[1];
    internal q23 = a[4] * b[2];
    internal q24 = a[5] * b[3];

    internal q25 = a[4] * b[0];
    internal q26 = a[5] * b[1];
    internal q27  = a[0] * b[2];
    internal q28  = a[1] * b[3];
    internal q29  = a[2] * b[4];
    internal q30  = a[3] * b[5];

    internal q31  = a[0] * b[3];
    internal q32  = a[1] * b[4];
    internal q33  = a[2] * b[5];
    internal q34  = a[3] * b[0];
    internal q35  = a[4] * b[1];
    internal q36  = a[5] * b[2];



    internal c1 = q1 ^ r0 ^ q7 ^ q13 ^ r5 ^ q19 ^ q25 ^ r6 ^ q31;
    internal c2 = q2 ^ r1 ^ q8 ^ q14 ^ r0 ^ q20 ^ q26 ^ r7 ^ q32;
    internal c3 = q3 ^ r2 ^ q9 ^ q15 ^ r1 ^ q21 ^ q27 ^ r8 ^ q33;
    internal c4 = q4 ^ r3 ^ q10 ^ q16 ^ r2 ^ q22 ^ q28 ^ r9 ^ q34;
    internal c5 = q5 ^ r4 ^ q11 ^ q17 ^ r3 ^ q23 ^ q29 ^ r10 ^ q35;
    internal c6 = q6 ^ r5 ^ q12 ^ q18 ^ r4 ^ q24 ^ q30 ^ r11 ^ q36;

    internal[6] res;
    res[0] = c1;
    res[1] = c2;
    res[2] = c3;
    res[3] = c4;
    res[4] = c5;
    res[5] = c6;

    return res;
}


internal[3][n] example(share[3][n] a) {
    internal[3][n] res;
    internal[n] b0;
    b0 = Refresh(a[1]);
    res[0] = parallel(a[0], b0);
    internal[n] b2;
    b2 = XOR(a[0], a[1]);
    internal[n] b4;
    b4 = XOR(a[1], a[2]);
    internal[n] b3;
    res[1] = parallel(b2, b4);
    internal[n] b5;
    res[2] = parallel(b2, a[2]);
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