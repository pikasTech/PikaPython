import pika_cjson

data1 = "{\"data\":{\"requestSocialInsuranceFromYangCheng\":\"\",\"authenticationComparison\":\"no\",\"startupLogo\":\"4\",\"cardType\":\"00,01,02,03,04\",\"synfromhis\":\"no\",\"alarmThresholdValue\":\"37.2\",\"hospitalName\":\"余杭农贸市场\",\"facediscernMode\":\"01\",\"hospitalCode\":\"102\"},\"success\":true,\"resultCode\":\"0000\",\"time\":\"2022-05-20 14:10:27\",\"message\":\"操作成功\"} "

a = pika_cjson.Parse(data1)
a.print()
