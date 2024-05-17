import RPi_I2C_driver
import RPi.GPIO as GPIO
import time

mylcd = RPi_I2C_driver.lcd()
mylcd.backlight(1)
mylcd.lcd_clear()
time.sleep(1)

# Define pins
L1 = 6
L2 = 13
L3 = 19
L4 = 26
C1 = 12
C2 = 16
C3 = 20
C4 = 21
button_pin = 18


# Set up GPIO pins
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

# Set pins as output or input with pull-down resistor
for pin in [L1, L2, L3, L4]:
    GPIO.setup(pin, GPIO.OUT)

for pin in [C1, C2, C3, C4]:
    GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

# Setup button pin as input with pull-up resistor
GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

# Variable to track program execution state
program_running = True

def display_prompt(): # this function will display my prompt
    mylcd.lcd_display_string("Enter OBJcode:", 1) 
#this function will make sure user input is written on the second line     
def display_entered_code(code):
    mylcd.lcd_display_string(code, 2) 
    
# this defines a function to read a line of characters
def read_line(line, characters, received_str):
    global program_running

   

    if program_running:
        GPIO.output(line, GPIO.HIGH)

        if GPIO.input(C1) == 1:
            received_str += characters[0]
            mylcd.lcd_write_char(ord(characters[0]))

        if GPIO.input(C2) == 1:
            received_str += characters[1]
            mylcd.lcd_write_char(ord(characters[1]))

        if GPIO.input(C3) == 1:
            received_str += characters[2]
            mylcd.lcd_write_char(ord(characters[2]))


        if GPIO.input(C4) == 1:
            received_str += characters[3]
            mylcd.lcd_write_char(ord(characters[3]))
        #when the button is pressed this line of code will detect it and set the program running to false so that
        # so that it no longer accepts user input 
        if GPIO.input(button_pin) == GPIO.HIGH: 
           program_running = False
 

        GPIO.output(line, GPIO.LOW)

    return received_str

 #ops array   
ops = [
    "18", "58", "90", "40", "B4", "28",
    "88", "A0", "24", "64", "9C", "C4",
    "C0", "F4", "3C", "30", "34", "38",
    "48", "00", "68", "50", "70", "08",
    "6C", "74", "04", "D0", "20", "60",
    "98", "C8", "44", "D8", "AC", "4C",
    "A4", "A8", "F0", "EC", "0C", "78",
    "54", "80", "D4", "14", "7C", "E8",
    "84", "10", "1C", "5C", "94", "B0",
    "E0", "F8", "2C", "B8", "DC"
]

format2 = [
    False, False, True, False, True, False,
    False, True, False, False, True, False,
    False, False, False, False, False, False,
    False, False, False, False, False, False,
    False, False, False, False, False, False,
    True, False, False, False, True, False,
    True, True, False, False, False, False,
    False, False, False, False, False, True,
    False, False, False, True, False
]

mnemonics = [
    "ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP",
    "COMPF", "COMPR", "DIV", "DIVF", "DIVR", "FIX",
    "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT",
    "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL",
    "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
    "MULR", "NORM", "OR", "RD", "RMO", "RSUB",
    "SHIFTL", "SHIFTR", "SIO", "SSK", "STA", "STB",
    "STCH", "STF", "STI", "STL","STS", "STSW",
    "STT", "STX", "SUB", "SUBF", "SUBR", "SVC",
    "TD", "TIO", "TIX", "TIXR", "WD"
]
# this function will return the addressing mode and wether it is indexed or not 
def get_xbpe(temp_opcode):
    if len(temp_opcode) < 3:
        return "Invalid input"

    he_ch = temp_opcode[2]

    hex_value = 0

    if he_ch.isdigit():
        hex_value = int(he_ch)
    elif he_ch.isalpha():
        hex_value = ord(he_ch.upper()) - ord('A') + 10

    bit2 = (hex_value & 0x04) != 0
    bit1 = (hex_value & 0x02) != 0
# if the value is greater than or equal to 8 this means it is indexed
    if hex_value >= 8:
        if bit2 and not bit1: #b = 1 p=0
            return "B,X"
        elif not bit2 and bit1:#b = 0 p=1
            return "PC,X"
        elif bit2 and bit1:#b = 1 p=1
            return "INVALID"
        elif not bit2 and not bit1:#b = 0 p=0
            return "ABS,X"
    else:
        if bit2 and not bit1: # same as the previous except not indexed
            return "B"
        elif not bit2 and bit1:
            return "PC"
        elif bit2 and bit1:
            return "INVALID"
        elif not bit2 and not bit1:
            return "ABS"

def determine_format_3_or_4(temp_opcode):
    hex_digit = temp_opcode[2]

    if hex_digit.isdigit():
        hex_value = int(hex_digit)
    elif hex_digit.isalpha():
        hex_value = ord(hex_digit.upper()) - ord('A') + 10
# i made a very similar implementation in my pa1 basically checks if e is odd or even if it is odd it means format 4
    if hex_value % 2 == 0:
        return "3"
    else:
        return "4"

def get_nibits(hex_string):
    he_ch = hex_string[1]

    if he_ch.isdigit():
        hex_value = int(he_ch)
    elif he_ch.isalpha():
        hex_value = ord(he_ch.upper()) - ord('A') + 10
    else:
        return None
## this is my bitmask 
    hex_value &= 0x3

    if hex_value < 10:
        masked_char = str(hex_value)
    else:
        masked_char = chr(hex_value - 10 + ord('A'))

    result = list(hex_string)
    result[1] = masked_char

    masked_hex_string = ''.join(result)

    # Take only the first two characters of the masked_hex_string
    masked_hex_string = masked_hex_string[:2]
    if masked_hex_string[1] == "1":
        return "IMM"
    elif masked_hex_string[1] == "2":
        return "IND"
    else:
        return "SIM"


def get_mnemonic(hex_string): #this handles if its not format 2
    global ops, mnemonics

    he_ch = hex_string[1]

    if he_ch.isdigit():
        hex_value = int(he_ch)
    elif he_ch.isalpha():
        hex_value = ord(he_ch.upper()) - ord('A') + 10
    else:
        return None

    hex_value &= 0xFC

    if hex_value < 10:
        masked_char = str(hex_value)
    else:
        masked_char = chr(hex_value - 10 + ord('A'))

    result = list(hex_string)
    result[1] = masked_char

    masked_hex_string = ''.join(result)

    masked_hex_string = masked_hex_string[:2]
    # this piece will iterate over the mnemonics array
    try: 
        index = ops.index(masked_hex_string)
        return mnemonics[index]
    except ValueError:
        return None

def check_and_get_mnemonic(input_string): #checks for format 2
    if len(input_string) >= 2:
        prefix = input_string[:2]
        for i in range(len(ops)):
            if ops[i] == prefix and format2[i]:
                return mnemonics[i]
    return None

try:
    while True:
        received_code = ""
        display_prompt()
        
        while True:
            display_entered_code(received_code)
            received_code = read_line(L1, ["1", "2", "3", "A"], received_code)
            received_code = read_line(L2, ["4", "5", "6", "B"], received_code)
            received_code = read_line(L3, ["7", "8", "9", "C"], received_code)
            received_code = read_line(L4, ["F", "0", "E", "D"], received_code) # note that these are the keys mapped to the membrane

            if not program_running: # once the button is pressed this will execute
                mylcd.lcd_clear()
                mnemonic = check_and_get_mnemonic(received_code)
                if mnemonic:
                    format = "2"
                    mylcd.lcd_display_string(received_code + " F:" + format, 1)
                    mylcd.lcd_display_string(mnemonic, 2)
                else:
                    nibits = get_nibits(received_code) # this is just calling  a function taking the user input as its input will be same for the following lines of code
                    maskedmnemonic = get_mnemonic(received_code)
                    format = determine_format_3_or_4(received_code)
                    xbpe = get_xbpe(received_code)
                    if xbpe == "INVALID":
                        mylcd.lcd_display_string("INVALID CODE", 1)
                    else:
                        mylcd.lcd_display_string(received_code + " F:" + format + " " + xbpe, 1)
                        mylcd.lcd_display_string(maskedmnemonic + " " + nibits, 2)
                
                time.sleep(8)  # displays output for 8 seconds before clearing screen
                mylcd.lcd_clear() 
                program_running = True 
                break  # breaks inner loop to ask for new input

            time.sleep(0.3)

except KeyboardInterrupt:
    print("\n Program Terminated.")

finally:
    GPIO.cleanup()
