import pika_cjson

data1 = "{\"data\":{\"validTime\":28800,\"token\":\"3E6EA1D907B9CFEB6AB1DECB5667E4A7\"},\"success\":true,\"resultCode\":\"0000\"} "
a = pika_cjson.Parse(data1)
a.print()
