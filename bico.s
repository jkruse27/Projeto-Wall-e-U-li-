.globl puts
.globl set_time
.globl get_time
.globl get_current_GPS_position
.globl get_us_distance
.globl get_gyro_angles
.globl set_head_servo
.globl set_engine_torque
.globl set_torque

set_time:
	li a7, 22
	ecall
	ret	

puts:
	# Encontra o tamanho da string
	mv t1, a0
	looplei:
		lbu t0, 0(t1)
		beqz t0, cont
		addi t1, t1, 1
		j looplei
	# Realiza a chamada de sistema
	cont: 
	mv a1, a0
	li a0, 0
	sub a2, t1, a1
	li a7, 64
	ecall	
	ret

get_time:
	li a7, 21
	ecall
	ret

get_current_GPS_position:
	li a7, 19
	ecall
	ret

get_gyro_angles:
	li a7, 20
	ecall
	ret

get_us_distance:
	li a7, 16
	ecall
	ret

set_head_servo:
	li a7, 17
	ecall

	addi t0, x0, -1
	beq t0, a0, m_u
	addi t0, t0, -1
	beq t0, a0, m_d
	j returnesis
	m_u:
		addi a0, a0, -1
		j returnesis
	m_d:
		addi a0, a0, 1
	returnesis:
		ret

set_engine_torque:
	# Verifica se o torque fornecido esta dentro dos valores permitidos
	li t0, 100
	bgt a1, t0, not_good
	addi t0, t0,-200
	blt a1, t0, not_good
	# Realiza a chamada de sistema
	li a7, 18
	ecall
	beqz a0, endis
	li a0, -2
	j endis
	not_good:
		li a0, -1
	endis:
		ret

set_torque:
	# Verifica se os torques fornecidos esta dentro dos valores permitidos
	li t0, 100
	bgt a0, t0, bad_bad
	bgt a1, t0, bad_bad
	addi t0, t0, -200
	blt a0, t0, bad_bad
	blt a1, t0, bad_bad
	# Realiza a chamada de sistema
	mv a2, a1
	mv a1, a0
	li a0, 0
	li a7, 18
	ecall
	mv a1, a2
	li a0, 1
	ecall
	j endson

	bad_bad:
		li a0, -1
	endson:
		ret		
