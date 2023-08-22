import re
phone = "2004-959-559 # this is a phone number"
num = re.sub('#.*$', "", phone)
print("the phone number is: ", num)
num = re.sub('\\D', "", phone)
print("the phone number is: ", num)