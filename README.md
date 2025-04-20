# Serial_Modbus

This is a SERIAL CUSTOM MODBUS library.
Created mainly for communication with embedded devices in a serial line.

codes are seperated as SLAVE and MASTER.
## GENEL YAPI




### MEVCUT KOMUTLAR
buradaki komutlar master cihazin bakis acisina gore adlandirilmistir. Slave cihazlarda Write komutu okundugunda kendisinne bir sey yazilacagini analiz edecek sekilde dusunulmelidir. 

#### READ
#### WRITE
#### SYNC_WRITE
#### WRITE_ACK
#### REBOOT
#### 

## Master Struct
Master klasorundeki kodlar kutuphane olarak hazirlanmistir. Buradaki kodlar master cihazinda ne yapilmasi istendigine gore gerekli fonksiyonlar cagirilarak calistirilmalidir. hali hazirda bulunan kutuphaneler python olarak hazirlanmistir. Sinirli sayida komut icin yazilmis olan kutuphaneye komut eklemek mumkundur. Slave cihazlardaki protocol yapisina da ayni komutlar ve komutlara karsilik gelen eylemler entegre edilmelidir.

## Slave Struct
Buradaki slave tamamen masterdan gelen komutlara gore gerekni yapmasi icin hazirlandi. gomulu cihazlarin slave olacagi dusunuldugunden c ile hazirlanmis durumda. gelen pakete gore kontrollerin ardindan komuta gore yapmasi gerekenin yapilmasi uzerine protocol fonksiyonalari calisacaktir. 

gelen paket circular buffer a aktarilmali ve ayni zamanda interruptlar ile paket geldiginde protocol fonksiyonlarinin calismasi saglanmalidir.

