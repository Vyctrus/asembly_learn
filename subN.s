
SYSEXIT32 = 1 # nr funkcji restartu (=1) – zwrot sterowania do s.o. 
SYSCALL32 = 0x80 # sysfun: nr funkcji: %eax, parametry: %ebx,%ecx,%edx 
EXIT_SUCCES = 0

SYSREAD32 = 3
SYSWRITE32 = 4
STDIN = 0
STDOUT = 1

BUFLEN = 512

.data
	liczba1: .long 0x10304008, 0x701100FF, 0x45100020, 0x08570030 
	# dowolnie duze czyli moze ich tutaj byc sporo
	# liczba1: .long 0x8, 0xF, 0x0, 0x3, 0x5
	# liczba1: .long 0x5, 0xFFFFFFFF, 0xFFFFFFFF
	size1 = . - liczba1
	liczba2: .long 0xF040500C, 0x00220026, 0x321000CB, 0x04520031 
	# liczba2: .long 0x3, 0x4
	size2 = . - liczba2
.bss
	.comm textout, 512

.text
.global _start
_start:
	# OPIS: liczba1 i liczba2 podawane så jako tablice liczb 4bajtowych
	# Dzielimy przez 4(Bajty) aby uzyskacrozmiar tabeli danej liczby
	# Korzystamy z ". - liczba1" aby sprawdzic jak wiele pamieci zajmuje liczba1
	clc # czyszczenie flagi przeniesieni CF, CF ← 0;
	mov $size1, %eax
	mov $4, %ebx
	div %ebx # eax/ebx= Y  eax<-Y
	movl %eax, %esi # ustawic break poin

	mov $size2, %eax
	mov $4, %ebx
	div %ebx # eax/ebx= Y  eax<-Y
	movl %eax, %edi
	
	cmpl %edi, %esi # ktora liczba ma wiecej liczb
	jle set_array_size #if(esi<=edi){set_array_szie}else{ omin_to } zdebudowane
	# esi>edi
	movl %esi, %ecx # ustalenie indexu do iterowania
	jmp dodaj

	jmp omin_to_1
		# esi<=edi
		set_array_size:
			mov %edi, %ecx
			jmp dodaj
	omin_to_1:

dodaj:
	
	clc
petla:
	# indexed adresssing- opis https://gist.github.com/mishurov/6bcf04df329973c15044
	# base_address(offset_address, index, size)
	# base_address + offset_address + index * size
	dec %esi
	dec %edi
	pushf
	# if esi <0 skonczyla sie liczba1
	cmpl $0, %esi 
	jl liczba1_zero # zdebudowane: ok
	# esi>=0
	movl liczba1(,%esi,4),%eax # odczytaj z pamieci
	jmp omin_to_3
		# esi<0
		liczba1_zero:
			movl $0,%eax # dopelnij zerami
	omin_to_3:

	# if edi <0 skonczyla sie liczba2
	cmpl $0, %edi 
	jl liczba2_zero # zdebudowane: ok
	# esi>=0
	movl liczba2(,%edi,4),%ebx # odczytaj z pamieci
	jmp omin_to_4
		# esi<0
		liczba2_zero:
			movl $0,%ebx # dopelnij zerami
	omin_to_4:
	popf

	subl %ebx, %eax # DEST ← DEST + SRC + CF;(dest,src 2nd)  AT&T -> (src, dst)		
	pushL %eax # wrzuc fragment wyniku na stos
	loop petla # iteruj po ecx # Loop until the ECX register is zero


jc add_carry_to_highest # Jump short if carry (CF=1).
# pushf
pushl $0x0
jmp omin_to_2
	add_carry_to_highest:
	pushl $0xF
	# pushf
omin_to_2:
clc

result:
	mov $size1, %eax
	mov $4, %ebx
	div %ebx # eax/ebx= Y  eax<-Y
	movl %eax, %esi # ustawic break poin
	
	mov $size2, %eax
	mov $4, %ebx
	div %ebx # eax/ebx= Y  eax<-Y
	movl %eax, %edi
	
	cmpl %edi, %esi # ktora liczba ma wiecej liczb
	jle heep_size
	# esi>edi
	movl %esi, %ecx # ustalenie indexu do iterowania
	jmp omin_to_end
		# esi<=edi
		heep_size:
			mov %edi, %ecx
	omin_to_end:

	
	pop %eax # pop carry from highest position	

	my_pop_loop:
		popl %eax
		# wysc na wyjscie?
	loop my_pop_loop

	
koniec:
# WYJSCIE Z PROGRAMU
mov $SYSEXIT32, %eax
mov $EXIT_SUCCES, %ebx
int $SYSCALL32
