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
internal[3] example(internal[3] a, internal[3] b) {
    internal[3] e = RefreshM(b);
    internal[3] c = XOR(a, e);
    internal[3] d = parallel(a, c);
    return d;
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