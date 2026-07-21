from math import pow

table = ""
for n in range(128):
    freq = 440.0 * pow(2, (n - 69) / 12)
    
    if n % 12 == 0:
        table += '\t\t'
        
    table += f"{freq:9.3f}"
    
    if n < 127:
        table += ", "
    
    if n % 12 == 11:
        table += '\n'
        

with open("MidiFreq_temp.h", "r") as file:
    code = file.read()
    
code = code.replace("/*...*/", table)

with open("MidiFreq.h", "w") as file:
    file.write(code)
    
print(code)
