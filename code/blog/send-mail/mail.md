## 使用python发送邮件


使用该脚本可以进行邮件发送，目前，该脚本使用的服务器为腾讯企业邮箱，暂不支持更多扩展。

使用方法如下：

```
$ MAIL_USERNAME=username MAIL_PASSWORD=password ./mail.py "第一封邮件"  "`cat mail.html`"
```

其中`mail.py`为脚本本身，`MAIL_USERNAME` 和 `MAIL_PASSWORD`为环境变量，`mail.html`为邮件的内容。


`mail.py`脚本如下：
```
#!/usr/bin/env python
from email.Header import Header
from email.MIMEMultipart import MIMEMultipart
from time import ctime
from smtplib import SMTP_SSL
from email import header
from email.MIMEText import MIMEText
import datetime, time
import sys, os

try:
    username  = os.environ['MAIL_USERNAME']
    password = os.environ['MAIL_PASSWORD']
except KeyError:
    print('MAIL_USERNAME and MAIL_PASSWORD need to be set')
    exit(1)

def send_mail(server, port,  user, pwd, frm, to, cc, subject, context, context_type='text', attach=None):
    outer = MIMEMultipart('alternative')
    outer['Date']   = ctime()
    outer['From']   = frm
    outer['To']     = to
    outer['Cc']     = cc
    outer['subject']= Header(subject, 'utf-8')
    msg = MIMEText(context, _subtype=context_type, _charset="utf-8") # _subtype='html'
    outer.attach(msg)
    to_list = outer["to"].split(",")+outer["cc"].split("c")
    real_to_list = set()
    for to in to_list:
        to = to.strip()
        if '@' in to:
                real_to_list.add(to)
    real_to_list = tuple(real_to_list)

    send_server = SMTP_SSL(server, port)
    send_server.login(user, pwd)
    errors = send_server.sendmail(frm, real_to_list, outer.as_string())
    send_server.quit()

if __name__ == '__main__':
    mail_server_address = 'smtp.exmail.qq.com'
    mail_server_port = '465'
    mail_server_user = username
    mail_server_pwd = password
    mail_server_sender= username

    to = 'w@laoqinren.net'
    cc = 'w@laoqinren.net'
    date = time.strftime("%Y-%m-%d")

    subject = sys.argv[1]
    content = sys.argv[2]
    content_type = 'html'
    send_mail(mail_server_address, mail_server_port, mail_server_user, mail_server_pwd, mail_server_sender, to, cc, subject, content, content_type)
```
