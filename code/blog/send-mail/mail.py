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
