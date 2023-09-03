n = 4;

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

internal[4] compression(internal[4] a, internal[4] b) {
    random r0;
    random r1;
    random r2;
    random r3;
    internal t1 = a[0] * b[0];
    internal t2 = t1 ^ r0;
    internal t3 = a[0] * b[3];
    internal t4 = t2 ^ t3;
    internal t5 = a[3] * b[0];
    internal t6 = t4 ^ t5;
    internal t7 = t6 ^ r1;
    internal t8 = a[0] * b[2];
    internal t9 = t7 ^ t8;
    internal t10 = a[2] * b[0];
    internal t11 = t9 ^ t10;

    internal t12 = a[1] * b[1];
    internal t13 = t12 ^ r2;
    internal t14 = a[1] * b[3];
    internal t15 = t13 ^ t14;
    internal t16 = a[3] * b[1];
    internal t17 = t15 ^ t16;
    internal t18 = t17 ^ r1;
    internal t19 = a[1] * b[2];
    internal t20 = t18 ^ t19;
    internal t21 = a[2] * b[1];
    internal t22 = t20 ^ t21;


    internal t23 = a[2] * b[2];
    internal t24 = t23 ^ r3;
    internal t25 = a[2] * b[3];
    internal t26 = t24 ^ t25;
    internal t27 = a[3] * b[2];
    internal t28 = t26 ^ t27;

    internal t29 = a[3] * b[3];
    internal t30 = t29 ^ r3;
    internal t31 = t30 ^ r2;
    internal t32 = t31 ^ r0;
    internal t33 = a[0] * b[1];
    internal t34 = t32 ^ t33;
    internal t35 = a[1] * b[0];
    internal t36 = t34 ^ t35;

    internal[4] res;
    res[0] = t11;
    res[1] = t22;
    res[2] = t28;
    res[3] = t36;

    return res;
}

internal[n] example(share[n] a, share[n] b) {
    internal[n] z;
    z = XOR(a, b);
    internal[n] c;
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