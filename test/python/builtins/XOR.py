# '6' in binary is '110', '3' in binary is '011'.
# Bitwise XOR operation: '110' ^ '011' = '101' = '5' in decimal
assert 6 ^ 3 == 5

# '10' in binary is '1010', '6' in binary is '0110'.
# Bitwise XOR operation: '1010' ^ '0110' = '1100' = '12' in decimal
assert 10 ^ 6 == 12

# Start with '6' ('110' in binary)
value = 6

# Bitwise XOR and assign with '3' ('011' in binary), '110' ^ '011' = '101'
value ^= 3
assert value == 5

# Bitwise XOR and assign with '10' ('1010' in binary), '101' ^ '1010' = '1111'
value ^= 10
assert value == 15


print('PASS')
