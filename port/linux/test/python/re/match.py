import re

line = "Cats are smarter than dogs"

m = re.match("(.*) are (.*?) .*", line, re.M | re.I)

if m:
    print("matchObj.group(0) : ", m.group(0))
    print("matchObj.group(1) : ", m.group(1))
    print("matchObj.group(2) : ", m.group(2))
else:
    print("No match!!")
