
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

internal[n] XOR(internal[n] a, internal[n] b) {
	internal[n] res;
	int i;
  	for (i = 0; i <= n - 1; i = i + 1) {
  		res[i] = a[i] ^ b[i];
  	}
  	return res;
}
internal[n] NOT(internal[n] a) {
    internal[n] res;
    res[0] = ~ a[0];
	int i;
  	for (i = 1; i <= n - 1; i = i + 1) {
  		res[i] = a[i];
  	}
  	return res;
}

internal[n] SecMult(share[n] a, share[n] b) {
	internal[n] c;
	int i;
    for(i = 0; i <= n - 1; i = i + 1) {
           c[i] = a[i] * b[i];
    }

    int j;
    for(i = 0; i <= n - 1; i = i + 1) {
        for(j = i + 1; j <= n - 1; j = j + 1) {
            random s;
            internal t1 = a[i] * b[j];
            internal t2 = s ^ t1;
            internal t3 = a[j] * b[i];
            internal ss = t2 ^ t3;
            c[i] = c[i] ^ s;
            c[j] = c[j] ^ ss;
        }
    }

	return c;
}

internal[n] sbox(internal[8][n] a) {
   internal t8 = XOR(a[3], a[5]);
   internal t9 = XOR(a[0], a[6]);
   internal t10 = XOR(t9, t8);
   internal t11 = XOR(a[0], a[3]);
   internal t12 = XOR(a[0], a[5]);
   internal t13 = XOR(a[1], a[2]);
   internal t14 = XOR(t13, a[7]);
   internal t15 = XOR(t14, a[3]);
   internal t16 = XOR(t14, a[0]);
   internal t17 = XOR(t14, a[6]);
   internal t18 = XOR(a[4], t10);
   internal t19 = XOR(t17, t12);
   internal t20 = XOR(t18, a[5]);
   internal t21 = XOR(t18, a[1]);
   internal t22 = XOR(t20, a[7]);
   internal t23 = XOR(t20, t13);
   internal t48 = compression(t12, t23);
   internal t24 = XOR(t21, t11);
   internal t25 = XOR(a[7], t24);
   internal t26 = XOR(t23, t24);
   internal t27 = XOR(t23, t12);
   internal t28 = XOR(t13, t24);
   internal t29 = XOR(t9, t28);
   internal t30 = XOR(a[0], t28);
   internal t31 = compression(t10, t20);
   internal t33 = compression(t15, a[7]);
   internal t34 = compression(t9, t28);
   internal t36 = compression(t16, t25);
   internal t37 = compression(t11, t24);
   internal t38 = compression(t8, t26);
   internal t32 = compression(t19, t22);
   internal t35 = compression(t17, t14);
   internal t39 = XOR(t32, t31);
   internal t40 = XOR(t33, t31);
   internal t41 = XOR(t35, t34);
   internal t42 = XOR(t36, t34);
   internal t43 = XOR(t38, t37);
   internal t44 = XOR(t39, t43);
   internal t45 = XOR(t41, t43);
   internal t46 = XOR(t44, t21);
   internal t47 = XOR(t45, t29);
   internal t49 = parallel(t46, t47);
   internal t50 = XOR(t48, t37);
   internal t51 = XOR(t40, t50);
   internal t52 = XOR(t42, t50);
   internal t53 = XOR(t52, t30);
   internal t54 = XOR(t47, t53);
   internal t55 = XOR(t51, t27);
   internal t56 = XOR(t46, t55);
   internal t57 = XOR(t53, t49);
   internal t58 = XOR(t55, t49);
   internal t59 = parallel(t56, t57);
   internal t60 = parallel(t58, t54);
   internal t61 = XOR(t59, t55);
   internal t62 = XOR(t60, t53);
   internal t63 = XOR(t47, t62);
   internal t64 = XOR(t57, t62);
   internal t65 = XOR(t61, t62);
   internal t66 = SecMult(t61, t16);
   internal t67 = parallel(t53, t64);
   internal t68 = XOR(t67, t63);
   internal t69 = XOR(t57, t67);
   internal t70 = compression(t61, t69);
   internal t71 = SecMult(t61, t25);
   internal t72 = XOR(t62, t68);
   internal t73 = XOR(t56, t70);
   internal t74 = XOR(t73, t68);
   internal t75 = XOR(t61, t73);
   internal t76 = XOR(t65, t74);
   internal t77 = SecMult(t72, t20);
   internal t78 = SecMult(t68, t22);
   internal t79 = SecMult(t72, t10);
   internal t80 = SecMult(t68, t19);
   internal t81 = SecMult(t73, t14);
   internal t82 = SecMult(t65, t24);
   internal t83 = SecMult(t73, t17);
   internal t84 = SecMult(t65, t11);
   internal t85 = SecMult(t76, t26);
   internal t86 = SecMult(t74, t23);
   internal t87 = SecMult(t76, t8);
   internal t88 = SecMult(t74, t12);
   internal t89 = SecMult(t62, t15);
   internal t90 = SecMult(t75, t9);
   internal t91 = SecMult(t62, a[7]);
   internal t92 = SecMult(t75, t28);
   internal t93 = XOR(t84, t87);
   internal t94 = XOR(t80, t89);
   internal t95 = XOR(t71, t83);
   internal t96 = XOR(t79, t80);
   internal t97 = XOR(t91, t90);
   internal t98 = XOR(t91, t71);
   internal t99 = XOR(t85, t86);
   internal t100 = XOR(t77, t92);
   internal t101 = XOR(t82, t85);
   internal t102 = XOR(t87, t88);
   internal t103 = XOR(t90, t95);
   internal t104 = XOR(t97, t100);
   internal t105 = XOR(t81, t93);
   internal t106 = XOR(t92, t101);
   internal t107 = XOR(t93, t104);
   internal t108 = XOR(t66, t104);
   internal t109 = XOR(t99, t105);
   internal t110 = XOR(t96, t105);
   internal t111 = XOR(t81, t106);
   internal t112 = XOR(t108, t109);
   internal t113 = XOR(t78, t110);
   internal t114 = XOR(t106, t110); #//
   internal t115 = XOR(t103, t109);
   internal t116 = XOR(t95, t107);
   internal t117 = XOR(t111, t112);
   internal t118 = XOR(t100, t113); #//
   internal t119 = XOR(t98, t113); #//
   internal t120 = XOR(t94, t112); #//
   internal t121 = XOR(t111, t118);
   internal t122 = XOR(t102, t117);
   internal t123 = NOT(t115); #//
   internal t124 = NOT(t116); #//
   internal t125 = NOT(t121); #//
   internal t126 = NOT(t122); #//

   return t126;

}

internal main(private[8] k) {
    internal[8][n] Key;
    for(i = 0; i <= 7; i = i + 1) {
        Key[i] = preshare(k[i]);
    }
    internal[3] res;
    res = sbox(Key);
    return res;
}

