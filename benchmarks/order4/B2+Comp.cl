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