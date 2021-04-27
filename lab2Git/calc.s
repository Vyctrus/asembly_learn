SYSEXIT32 = 1
SYSCALL32 = 0x80 
EXIT_SUCCES = 20

.section .text

.section .data
    num_a: .float -10.003 # .double 10.01
    num_b: .float -0.0 # -5.12

    control_word: .float 0
.text
.globl _start

_start:
    # Sets the FPU control, status, tag, instruction pointer, and data pointer registers to their default states.
    FINIT

cw_settings:
    # zaladownie rejetru CW do rejestru ecx
    FSTCW control_word
    MOV control_word, %ecx

    # 2.W programie należy swobodnie operować ustawianiem precyzji obliczeń, zaokrągleniami
    # oraz notować wyjątki (należy wykonać testy z różnymi ustawieniami)

    #KONTROLA PRECYZJI (Precision Control PC)
    # 8.1.5.2 Precision Control Field- bits 8 and 9 of the x87 FPU control word

    # WYMAGANE OPCJE============================
    # Single Precision (24 bits) 00B - float 
    # Double Precision (53 bits) 10B - double
    # ==========================================
    # Pozostale
    # Reserved 01B
    # Double Extended Precision (64 bits) 11B
    # ==========================================
    # "liczb pojedynczej i podwójnej precyzji"
    AND $0b1111110011111111, %ecx # wyzeruj maska 8,9 bit ecx
    # WYBOR PRECYZJI
    # OR  $0b0000000000000000, %ecx # float
     OR  $0x0000001000000000b, %ecx # double
    
    #KONTROLA ZAOKRAGLEN (Rounding Control RC)
    # The rounding-control (RC) field of the x87 FPU control register (bits 10 and 11)
    # page 106 intel doc, table 4-8

    AND $0b1111001111111111, %ecx # wyzeruj maska 10,11 bit

    # WYBOR ZAOKRAGLEN
    #  OR  $0b0000000000000000, %ecx # Round to nearest 00B
    # OR  $0b0000010000000000, %ecx # Round down (-> -∞) 01B
    # OR  $0b0000100000000000, %ecx # Round up (-> +∞) 10B
     OR  $0b0000110000000000, %ecx # Truncate (-> 0) 11B

    # ecx-> control word, then load control word
    MOV %ecx, control_word
    FLDCW control_word

    # JMP addition
    # JMP substraction
    # JMP multi
    # JMP division
    # JMP exceptions
    JMP NaN

# num_a + num_b
addition:
    # Load Floating Point Value
    FLD num_b
    FLD num_a
    # Add ST(0) to ST(1), store result in ST(1), and pop the register stack.
    FADDP # wynik jest pop?

# num_a - num_b
substraction:
    FLD num_b
    FLD num_a
    FSUBP
    JMP koniec

# num_a * num_b
multi:
    FLD num_b
    FLD num_a
    FMULP
    JMP koniec

# num_a / num_b
division:
    FLD num_b
    FLD num_a
    FDIVP

    JMP koniec

# pierwiastek drugiego stopnia z ujemnej liczby
NaN:
    FLD num_b
    FLD num_a
    FSQRT
    JMP koniec

koniec:
# WYJSCIE Z PROGRAMU
mov $SYSEXIT32, %eax
mov $EXIT_SUCCES, %ebx
int $SYSCALL32

