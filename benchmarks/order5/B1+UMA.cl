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


internal[6] uma(internal[6] a, internal[6] b) {
    internal[6] q;
    q[0] = a[0] * b[0];
    q[1] = a[1] * b[1];
    q[2] = a[2] * b[2];
    q[3] = a[3] * b[3];
    q[4] = a[4] * b[4];
    q[5] = a[5] * b[5];

    # i = 0;
    random r0;
    random r1;
    random r2;
    random r3;
    random r4;
    random r5;

    q[0] = q[0] + r0 + a[0] * b[5] + a[5] * b[0] + r5 + a[0] * b[4] + a[4] * b[0];
    q[1] = q[1] + r1 + a[1] * b[0] + a[0] * b[1] + r0 + a[1] * b[5] + a[5] * b[1];
    q[2] = q[2] + r2 + a[2] * b[1] + a[1] * b[2] + r1 + a[2] * b[0] + a[0] * b[2];
    q[3] = q[3] + r3 + a[3] * b[2] + a[2] * b[3] + r2 + a[3] * b[1] + a[1] * b[3];
    q[4] = q[4] + r4 + a[4] * b[3] + a[3] * b[4] + r3 + a[4] * b[2] + a[2] * b[4];
    q[5] = q[5] + r5 + a[5] * b[4] + a[4] * b[5] + r4 + a[5] * b[3] + a[3] * b[5];

    random r10;
    random r11;
    random r12;

    q[0] = q[0] + r10 + a[0] * b[3];
    q[1] = q[1] + r11 + a[1] * b[4];
    q[2] = q[2] + r12 + a[2] * b[5];
    q[3] = q[3] + r10 + a[3] * b[0];
    q[4] = q[4] + r11 + a[4] * b[1];
    q[5] = q[5] + r12 + a[5] * b[2];

    return q;

}

internal[n] example(internal[n] a, internal[n] b) {
    internal[n] e = RefreshM(b);
    internal[n] c = XOR(a, e);
    internal[n] d = uma(a, c);
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