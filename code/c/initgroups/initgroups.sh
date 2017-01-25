$ cat /etc/group	# 查看group数据库
root:x:0:
adm:x:4:syslog
docker:x:999:
vboxusers:x:112:
kvm:x:113:
ssl-cert:x:114:
scanner:x:115:
shellinabox:x:118:
mongodb:x:119:mongodb
lxc-dnsmasq:x:120:
# 设置当前进程的组列表为mongodb用户所在的组
$ ./initgroups  mongodb  
(0) group ID: 119
(1) group ID: 1234
# 设置当前进程的组列表为syslog用户所在的组
$ ./initgroups  syslog
(0) group ID: 4
(1) group ID: 1234
