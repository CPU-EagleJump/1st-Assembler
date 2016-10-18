	addi	x2, x0, 0	# li x2, 0
	addi	x3, x0, 30	# li x3, 30
	jal	x1, fib		# jal fib / call fib
	halt
fib:
	addi	x4, x0, 1	# li x4, 2
	blt	x4, x3, else
	jalr	x0, x1, 0	# ret
else:
	sw	x1, 0(x2)
	sw	x3, 4(x2)
	addi	x3, x3, -1
	addi	x2, x2, 8
	jal	x1, fib		# jal fib / call fib
	lw	x4, -4(x2)
	sw	x3, -4(x2)
	addi	x3, x4, -2
	jal	x1, fib		# jal fib / call fib
	addi	x2, x2, -8
	lw	x1, 0(x2)
	lw	x4, 4(x2)
	add	x3, x3, x4
	jalr	x0, x1, 0	# ret

