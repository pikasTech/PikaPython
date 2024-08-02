class scr_Text:
    def cus_print(self, data, end='\n'):
        str_buf = ""

        if isinstance(data, str):
            str_buf = str_buf + data
        elif isinstance(data, int):
            str_buf = str_buf + str(data)
        else:
            str_buf = "The type is not recognized"
        str_buf = str_buf + end


screen = scr_Text()
screen.cus_print(12)
screen.cus_print(12)
screen.cus_print(12)
screen.cus_print(12)

print('PASS')
