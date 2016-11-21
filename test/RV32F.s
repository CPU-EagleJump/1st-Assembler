# R-type
	fadd.s	f1, f1, f1
	fsub.s	f1, f1, f1
	fmul.s	f1, f1, f1
	fdiv.s	f1, f1, f1
	fsqrt.s	f1, f1
	fsgnj.s	f1, f1, f1
	fsgnjn.s	f1, f1, f1
	fsgnjx.s	f1, f1, f1
	fmin.s	f1, f1, f1
	fmax.s	f1, f1, f1
	fcvt.w.s	x1, f1
	fcvt.wu.s	x1, f1
	feq.s	x1, f1, f1
	flt.s	x1, f1, f1
	fle.s	x1, f1, f1
	fcvt.s.w	f1, x1
	fcvt.s.wu	f1, x1

# I-type
	flw	f1, 4(x1)

# S-type
	fsw	f1, 4(x1)

