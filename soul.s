.globl _start
.set X_MASK, 0x3FF00000
.set Y_MASK, 0xFFC00
.set Z_MASK, 0x3FF
.set READ_POS, 0xFFFF0004
.set POS_X, 0xFFFF0008
.set POS_Y, 0xFFFF000C
.set POS_Z, 0xFFFF0010
.set ANGLE, 0xFFFF0014
.set TORQUE_2, 0xFFFF0018
.set TORQUE_1, 0xFFFF001A
.set SERVO_3, 0xFFFF001C
.set SERVO_2, 0xFFFF001D
.set SERVO_1, 0xFFFF001E
.set READ_US, 0xFFFF0020
.set US, 0xFFFF0024
.set GPT, 0xFFFF0100
.set GPT_I, 0xFFFF0104
.set UART_S, 0xFFFF0108
.set UART_VS, 0xFFFF0109
.set UART_R, 0xFFFF010A
.set UART_VR, 0xFFFF010B


int_handler:
	# ---------------------- Salva estado atual ------------------- #

	csrrw a0, mscratch, a0
	sw t0, 0(a0)
	sw t1, 4(a0)
	addi a0, a0, 8
	csrrw a0, mscratch, a0

	# Reabilita interrupts durante o tratamento
	csrr t1, mstatus
	ori t1, t1, 0x80
	csrw mstatus, t1

	csrr t1, mie
	li t0, 0x800
	or t1, t1, t0
	csrw mie, t1

	# ----------------- Identifica Interrupcao -------------------- #
	csrr t0, mcause
	bltz t0, timer
	li t1, 16
	beq a7, t1, read_us
	li t1, 17
	beq a7, t1, set_servo
	li t1, 18
	beq a7, t1, settorque
	li t1, 19
	beq a7, t1, gps
	li t1, 20
	beq a7, t1, read_gyro
	li t1, 21
	beq a7, t1, gettime
	li t1, 22
	beq a7, t1, settime
	li t1, 64
	beq a7, t1, write
	j return
	# ---------------- Tratamento da Interrupcao ------------------ #

	# Trata interrupcao do GPT a cada milisegundo atualizando o tempo do sistema
	timer:
		li t0, GPT_I
		lb t0, 0(t0)
		beq t0, zero, return
		la t0, tempo
		lw t1, 0(t0)
		addi t1, t1, 100
		sw t1, 0(t0)
		li t0, GPT_I
		sb zero, 0(t0)
		li t1, 100
		li t0, GPT
		sw t1, 0(t0)

		j return

	# Retorna a posicao x, y, z do robo no endereco fornecido em a0
	gps:
		li t0, READ_POS
		sw zero, 0(t0)
		loop:
			lw t1, 0(t0)
			beqz t1, loop
		li t0, POS_X
		lw t0, 0(t0)
		sw t0, 0(a0)
		li t0, POS_Y
		lw t0, 0(t0)
		sw t0, 4(a0)
		li t0, POS_Z
		lw t0, 0(t0)
		sw t0, 8(a0)

		j return_plus

	# Define o angulo de um dos servos edterminados do robo
	set_servo:
		beq a0, zero, s_zero
		li t0, 1
		beq a0, t0, s_um
		li t0, 2
		beq a0, t0, s_dois
		li a0, -2
		j end

		s_zero:
			li t0, 16
			blt a1, t0, bad_end
			li t0, 116
			bgt a1, t0, bad_end
			li t0, SERVO_1
			sb a1, 0(t0)
			j end
		s_um:
			li t0, 52
			blt a1, t0, bad_end
			li t0, 90
			bgt a1, t0, bad_end
			li t0, SERVO_2
			sb a1, 0(t0)
			j end

		s_dois:
			bltz a1, bad_end
			li t0, 156
			bgt a1, t0, bad_end
			li t0, SERVO_3
			sb a1, 0(t0)
			j end

		bad_end:
			li a0, -1
		end:
			j return_plus

	# Define o torque de um dos motores (a0) com o valor em a1
	settorque:
		beq zero, a0, m_zero
		li t0, 1
		beq t0, a0, m_um
		li a0, -1
		j end1

		m_um:
			li t0, TORQUE_2
			sh a1, 0(t0)
			li a0, 0
			j end1

		m_zero:
			li t0, TORQUE_1
			sh a1, 0(t0)
			li a0, 0

		end1:
			j return_plus

	# Retorna a leitura do sensor ultassom ou -1 caso nao haja nada no campo de visao (ou a menos de 600cm)
	read_us:
		li t1, READ_US
		sw zero, 0(t1)
		loop3:
			lw t0, 0(t1)
			beqz t0, loop3
		li t1, US
		lw t1, 0(t1)
		li a0, -1
		beq a0, t1, endi
		li t0, 600
		blt t1, t0, good
		li a0, -1
		j endi
		good:
			mv a0, t1
		endi:
			j return_plus

	# Le os angulos de Euler do robo e retorna no endereco fornecido
	read_gyro:
		li t0, READ_POS
		sw zero, 0(t0)
		loop1:
			# li a0, 49
			# jal print_num
			lw t1, 0(t0)
			beqz t1, loop1
		li t0, ANGLE
		lw t0, 0(t0)
		# li t1, X_MASK
		# and t1, t0, t1
		# srli t1, t1, 20
		slli t1, t0, 2
		srli t1, t1, 22
		sw t1, 0(a0)
		# li t1, Y_MASK
		# and t1, t0, t1
		# srli t1, t1, 10
		slli t1, t0, 12
		srli t1, t1, 22
		sw t1, 4(a0)
		# li t1, Z_MASK
		# and t1, t0, t1
		slli t1, t0, 22
		srli t1, t1, 22
		sw t1, 8(a0)

		j return_plus

	# Retorna o tempo do sistema
	gettime:
		la a0, tempo
		lw a0, 0(a0)
		j return_plus

	# Define o tempo do sistema
	settime:
		la t1, tempo
		sw a0, 0(t1)
		j return_plus

	write:
		csrrw a0, mscratch, a0
		sw t2, 0(a0)
		sw a1, 4(a0)
		sw a2, 8(a0)
		addi a0, a0, 12
		csrrw a0, mscratch, a0

		# Loop para escrever cada letra da string
		li t0, 0
		loop2:
			beqz a2, end3
			li t1, UART_VS
			lbu t2, 0(a1)
			sb t2, 0(t1)
			li t1, UART_S
			li t2, 1
			sb t2, 0(t1)
			# Loop esperando que se possa escrever outra letra
			lloop:
				lbu t2, 0(t1)
				bnez t2, lloop
			addi a1, a1, 1
			addi t0, t0, 1
			addi a2, a2, -1
			j loop2
		end3:
			mv a0, t0
			csrrw a0, mscratch, a0
			lw t2, 0(a0)
			lw a1, 4(a0)
			lw a2, 8(a0)
			addi a0, a0, -12
			csrrw a0, mscratch, a0
			j return_plus

	# ------------------------- Retorno --------------------------- #
	# Retorna para mepc + 4
	return_plus:
		csrr t0, mepc
		addi t0, t0, 4
		csrw mepc, t0
	return:		# Retorna para mepc
		csrrw a0, mscratch, a0
		lw t0, 0(a0)
		lw t1, 4(a0)
		addi a0, a0, -8
		csrrw a0, mscratch, a0

		mret

_start:
	# -------------------------- Configuracoes do Ambiente e Interrupcoes ----------------------------------- #

	# -------------------------------- Configuracoes do Hardware -------------------------------------------- #

	# Configura as configuracoes da cabeca para posicao natural (Base = 31, Mid = 80, Top = 78)
	li t0, SERVO_3
	li t1, 78
	sb t1, (t0)
	li t0, SERVO_2
	li t1, 80
	sb t1, (t0)
	li t0, SERVO_1
	li t1, 31
	sb t1, (t0)

	# Configura torque dos 2 motores para 0
	li t0, TORQUE_2
	sw zero, 0(t0)

	# Configura GPT para gerar interrupcoes a cada 1ms
	li t0, GPT
	li t1, 100
	sw t1, 0(t0)
	li t0, GPT_I
	sb zero, 0(t0)

	# -------------------- Tranferencia de Execucao para Aplicacao de Controle ------------------------------ #

	# Salva o endereco do tratador de interrupcoes/excessoes
	la t0, int_handler
	csrw mtvec, t0

	# Salva o ponteiro da pilha do sistema em mscratch
	la a0, pilha_s
	csrrw a0, mscratch, a0

	# Cria a pilha do programa no final da memoria (128000000)
	li sp, 0x7A12000

	# Habilita interrupcoes locais e globais
	csrr t1, mstatus
	ori t1, t1, 0x80
	csrw mstatus, t1

	csrr t1, mie
	li t0, 0x800
	or t1, t1, t0
	csrw mie, t1

	# Muda para o modo usuario e vai para o codigo do usuario (loco.c)
	csrr t1, mstatus
	li t0, ~0x1800
	and t1, t1, t0
	csrw mstatus, t1

	la t0, main
	csrw mepc, t0
	mret
l_i:
	j l_i


tempo: .word 0
pilha_s: .skip 1000
