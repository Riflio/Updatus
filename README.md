# Updatus
Update manager

# Versions

=== НОВОЕ В ВЕРСИИ 1.3 (08.06.2020) ===
        Добавили возможность ограничить количество потоков скачивания
        Сделали возможность несольких попыток скачивания в случае ошибки
        Добавили возможность только отобразить список того, что будет установлено
        Пофиксили продолжение установки и скачиваний при ошибках
        Пофиксили путь в кэше - теперь создаются подпапки

=== НОВОЕ В ВЕРСИИ 1.2 (11.11.2019) ===
        Сделали проверку доступа к директории
         Инициализацию и проверку доступа к основным папкам вынесли отдельно
         Теперь при ошибке не закрываем GUI, если было
         Пофиксили логи - теперь пишется в один файл сразу в заданной папке, если задана и есть возможность

=== НОВОЕ В ВЕРСИИ 1.1 ===
        Мелкие баг фиксы

=== НОВОЕ В ВЕРСИИ 1.0 ===
        Базовая версия



Example of updateManager.cnf:
```
[General]
defaultInstPath=tests # Install directory if not set instPath about every packet
lastUpdated=07.08.19 01:23 # Date and time last seccessful update
sendLogs=http://site.dev/updatus/handler/logHandler.php # Url to send logs (see ServerHandler directory)
tempDir=./updateCache/ # Dir to store updates
version=1.0 # This config file version
runAfter= # Program to start after run Updatus
logDir= # Directory to logs
downloadStreams=10 # Сoncurrent downloading streams
downloadAttempts=3 # Download attempts, if has error

[servers] # Servers list, with updates (see repository.cnf)
main=http://suvenirus.dev/updatemanager/ # At now, only one avaliable

[info] # All values will be send to server log handler
platform=linux
uuid=1239sdf124e

[installed] # What packadges need to be installed, after first update replaced by latest versions
packetA=2.0
packetB=1.9
packetC=4.0
packetD=9.0
packetF=22.4

[packetA:2.0]
cachePath=./updateCache/hcsterminal/linux/packetA/2.0/packetA.zip #Where placed
instPath=tests # Where should unpack this packet (and all new versions) at target machine
instType=asManual # Installation type. "asRels" may be deleted, if nobody has used hes
rels=  #Relatives this packet, format:  "name1:version1;name2:version2"

[packetB:1.9]
cachePath=./updateCache/hcsterminal/linux/packetB/1.9/packetB.zip
instPath=tests
instType=asManual
rels=packetF:22.4

[packetC:4.0]
cachePath=./updateCache/hcsterminal/linux/packetC/4.0/packetC.zip
instPath=tests
instType=asManual
rels="packetA:3.0;packetA:2.0;packetB:1.9"

[packetD:9.0]
cachePath=./updateCache/hcsterminal/linux/packetD/9.0/packetD.zip
instPath=tests
instType=asManual
rels="packetA:2.0;packetA:1.0;packetC:4.1;packetC:4.0"

[packetF:2.4]
cachePath=./updateCache/hcsterminal/linux/packetF/22.4/packetF.zip
instPath=tests
instType=asRels
rels=packetA:2.0
```
