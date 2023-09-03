#Tight Private Circuits: Achieving Probing Security with the Least Refreshing
# Fig. 1


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

internal[n] XOR(internal[n] a, internal[n] b) {
	internal[n] res;
	int i;
  	for (i = 0; i <= n - 1; i = i + 1) {
  		res[i] = a[i] ^ b[i];
  	}
  	return res;
}


internal[5] parallel(internal[5] a, internal[5] b) {
    random r0;
    random r1;
    random r2;
    random r3;
    random r4;
    internal q1 = a[0] * b[0];
    internal q2 = a[1] * b[1];
    internal q3 = a[2] * b[2];
    internal q4 = a[3] * b[3];
    internal q5 = a[4] * b[4];

    internal q6 = a[0] * b[4];
    internal q7 = a[1] * b[0];
    internal q8 = a[2] * b[1];
    internal q9 = a[3] * b[2];
    internal q10 = a[4] * b[3];

    internal q11 = a[4] * b[0];
    internal q12 = a[0] * b[1];
    internal q13  = a[1] * b[2];
    internal q14  = a[2] * b[3];
    internal q15  = a[3] * b[4];

    internal q16  = a[0] * b[3];
    internal q17  = a[1] * b[4];
    internal q18  = a[2] * b[0];
    internal q19  = a[3] * b[1];
    internal q20  = a[4] * b[2];

    internal q21 = a[3] * b[0];
    internal q22 = a[4] * b[1];
    internal q23  = a[0] * b[2];
    internal q24  = a[1] * b[3];
    internal q25  = a[2] * b[4];

    internal c1 = q1 ^ r0 ^ q6 ^ q11 ^ r4 ^ q16 ^ q21;
    internal c2 = q2 ^ r1 ^ q7 ^ q12 ^ r0 ^ q17 ^ q22;
    internal c3 = q3 ^ r2 ^ q8 ^ q13 ^ r1 ^ q18 ^ q23;
    internal c4 = q4 ^ r3 ^ q9 ^ q14 ^ r2 ^ q19 ^ q24;
    internal c5 = q5 ^ r4 ^ q10 ^ q15 ^ r3 ^ q20 ^ q25;

    internal[5] res;
    res[0] = c1;
    res[1] = c2;
    res[2] = c3;
    res[3] = c4;
    res[4] = c5;

    return res;
}
internal[n] example(share[n] a, share[n] b) {
    internal[n] z;
    z = XOR(a, b);
    internal[n] c;
    #c = SecMult(z, a);
    c = parallel(z, a);
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