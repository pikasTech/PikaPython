#!/bin/sh
ProjName="W806"
signature=0
prikey_sel=0
code_encrypt=0
sign_pubkey_src=0
img_type=1
zip_type=1

sec_img_header=8002000
sec_img_pos=8002400
run_img_header=8010000
run_img_pos=8010400
upd_img_pos=8010000


echo $ProjName
if [ $prikey_sel -gt 0 ]
 then
  let img_type=$img_type+32*$prikey_sel
fi

if [ $code_encrypt -eq 1 ]
 then
  let img_type=$img_type+16
fi

if [ $signature -eq 1 ]
 then
  let img_type=$img_type+256
fi

if [ $sign_pubkey_src -eq 1 ]
 then
  let img_type=$img_type+512
fi

echo $img_type

csky-elfabiv2-objcopy -O binary ./"$ProjName".elf ./"$ProjName".bin

if [ $code_encrypt -eq 1 ]
 then
  let prikey_sel=$prikey_sel+1
  openssl enc -aes-128-ecb -in ./"$ProjName".bin -out ./"$ProjName"_enc.bin -K 30313233343536373839616263646566 -iv 01010101010101010101010101010101
  openssl rsautl -encrypt -in ../Tools/ca/key.txt -inkey ../Tools/ca/capub_"$prikey_sel".pem -pubin -out key_en.dat
  cat "$ProjName"_enc.bin key_en.dat > "$ProjName"_enc_key.bin
  cat "$ProjName"_enc_key.bin ../Tools/ca/capub_"$prikey_sel"_N.dat > "$ProjName"_enc_key_N.bin  
  ../Tools/wm_tool.exe -b ./"$ProjName"_enc_key_N.bin -o ./"$ProjName" -it $img_type -fc 0 -ra $run_img_pos -ih $run_img_header -ua $upd_img_pos -nh 0  -un 0
 else
  ../Tools/wm_tool.exe -b ./"$ProjName".bin -o ./"$ProjName" -it $img_type -fc 0 -ra $run_img_pos -ih $run_img_header -ua $upd_img_pos -nh 0  -un 0
fi

mkdir -p ../Output
mv ./"$ProjName".map ../Output/"$ProjName".map

if [ $signature -eq 1 ]
 then
  openssl dgst -sign  ../Tools/ca/cakey.pem -sha1 -out "$ProjName"_sign.dat ./"$ProjName".img
  cat "$ProjName".img "$ProjName"_sign.dat > "$ProjName"_sign.img
  mv ./"$ProjName"_sign.img ../Output/"$ProjName"_sign.img

  #when you change run-area image's ih, you must remake secboot img with secboot img's -nh address same as run-area image's ih
  ../Tools/wm_tool.exe -b ../Tools/W806_secboot.bin -o ../Tools/W806_secboot -it 0 -fc 0 -ra $sec_img_pos -ih $sec_img_header -ua $upd_img_pos -nh $run_img_header  -un 0    
  cat ../Tools/"$ProjName"_secboot.img ../Output/"$ProjName"_sign.img > ../Output/"$ProjName".fls  
 else
  mv ./"$ProjName".img ../Output/"$ProjName".img
  
  #when you change run-area image's ih, you must remake secboot img with secboot img's -nh address same as run-area image's ih
  ../Tools/wm_tool.exe -b ../Tools/W806_secboot.bin -o ../Tools/W806_secboot -it 0 -fc 0 -ra $sec_img_pos -ih $sec_img_header -ua $upd_img_pos -nh $run_img_header  -un 0  
  cat ../Tools/"$ProjName"_secboot.img ../Output/"$ProjName".img > ../Output/"$ProjName".fls
fi

#produce compressed ota firmware*/
if [ $zip_type -eq 1 ]
 then
  if [ $signature -eq 1 ]
   then
    ../Tools/wm_tool.exe -b ./"$ProjName"_sign.img -o ../Output/"$ProjName"_sign -it $img_type -fc 1 -ra $run_img_pos -ih $run_img_header -ua $upd_img_pos -nh 0  -un 0
    mv ../Output/"$ProjName"_sign_gz.img ../Output/"$ProjName"_sign_ota.img
  else
   ../Tools/wm_tool.exe -b ../Output/"$ProjName".img -o ../Output/"$ProjName" -it $img_type -fc 1 -ra $run_img_pos -ih $run_img_header -ua $upd_img_pos -nh 0  -un 0
   mv ../Output/"$ProjName"_gz.img ../Output/"$ProjName"_ota.img
  fi
fi
#openssl --help