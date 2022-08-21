def is_include(included_data, data):     #判断一个字符串是否存在于另外一个字符串中
    if str(type(data)) == "<class 'str'>" and str(type(included_data)) == "<class 'str'>":
        print('type assert passed')
        data_length = len(data)
        included_data_length = len(included_data)
        if data_length == included_data_length:
            print('length assert passed')
            if data == included_data:
                print('data equal assert passed 1')
                return True
            else:
                print('data not equal 1')
                return None
        elif data_length > included_data_length:
            max_count = data_length - included_data_length
            for i in range(0, max_count):
                if data[i:i+included_data_length] == included_data:
                    print('data equal assert passed 2')
                    return True
            print('data not equal 2')
            return None
        else:
            print('data not equal 3')
            return None
    else:
        print('type assert failed')
        return 'TypeError'

a = '34'
b = '35'
ret = (is_include(a, b))
ret_str = str(ret)
print('ret = %s' % ret_str)
