	.data
long.0:
	.long 0x0
long.10:
	.long 0x10

	.text
# R-type
	slli	x1, x1, 1
	srli	x1, x1, 1
	srai	x1, x1, 1
	add	x1, x1, x1
	sub	x1, x1, x1
	sll	x1, x1, x1
	slt	x1, x1, x1
	sltu	x1, x1, x1
	xor	x1, x1, x1
	srl	x1, x1, x1
	sra	x1, x1, x1
	or	x1, x1, x1
	and	x1, x1, x1

# I-type
	addi	x1, x1, 1
	addi	x1, x1, -1
	addi	x1, x1, label
	slti	x1, x1, 1
	sltiu	x1, x1, 1
	xori	x1, x1, 1
	ori	x1, x1, 1
	andi	x1, x1, 1
	lw	x1, 4(x1)
	lw	x1, long.10(x0)	# static data
	jalr	x1, x1, 1

# S-type
	sw	x1, 4(x1)
	sw	x1, long.10(x0)	# static data

# SB-type
label:
	beq	x1, x1, label
	bne	x1, x1, label
	blt	x1, x1, label
	bge	x1, x1, label
	bltu	x1, x1, label
	bgeu	x1, x1, label

# U-type
	lui	x1, 1
	auipc	x1, 1

# UJ-type
	jal	x1, label

