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

internal[3] parallel(internal[3] a, internal[3] b) {
    random r0;
    random r1;
    random r2;
    internal q1 = a[0] * b[0];
    internal q2 = a[1] * b[1];
    internal q3 = a[2] * b[2];
    internal q4 = a[0] * b[2];
    internal q5 = a[1] * b[0];
    internal q6 = a[2] * b[1];
    internal q7 = a[2] * b[0];
    internal q8 = a[0] * b[1];
    internal q9 = a[1] * b[2];

    internal c1 = q1 ^ r0 ^ q4 ^ q7 ^ r2;
    internal c2 = q2 ^ r1 ^ q5 ^ q8 ^ r0;
    internal c3 = q3 ^ r2 ^ q6 ^ q9 ^ r1;

    internal[3] res;
    res[0] = c1;
    res[1] = c2;
    res[2] = c3;

    return res;
}

internal[3][n] example(share[3][n] a) {
    internal[3][n] res;
    #res[0] = SecMult(a[0], a[1]);
    res[0] = parallel(a[0], a[1]);
    internal[n] b2;
    b2 = XOR(a[0], a[1]);
    internal[n] b4;
    b4 = XOR(a[1], a[2]);
    internal[n] b3;
    #res[1] = SecMult(b2, b4);
    res[1] = parallel(b2, b4);
    internal[n] b5;
    #res[2] = SecMult(b2, a[2]);
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