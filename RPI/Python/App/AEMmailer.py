#!/usr/bin/python3

"""
Monitor message content:

* Normal Monitoring:
Poll Count : <number>
Disk Space Remaining : <number> MB

* Exiting on Disk space
Disk space limit reached, shutting down...
Poll Count : <number>
Disk Space Remaining : <number> MB

* Exiting on exception
Error condition, aborting...

"""

import smtplib,getpass,csv

class Error(Exception):
   """Base class for other exceptions"""
   pass

class NoPasswordException(Error):
   """Raised when the input value is blank"""
   pass

class MissingEmailAccountsException(Error):
   """Raised when the input value is blank"""
   pass

class AEMMailer:
   def __init__(self):
      self.initToListAndSender()
      if not self.toList or self.senderGmail == '':
         raise MissingEmailAccountsException
      self.login()
      self.subject     = 'AEM Monitor'
      self.msgTemplateBase = ['To: %s ' % ", ".join(self.toList),
                              'From: %s' % self.senderGmail,
                              'Subject: %s' % self.subject,
                              '',]
      self.msgBlankText     =  '%s'

   def initToListAndSender(self,filenName='./mail.csv'):
      with open('mail.csv', newline='') as csvFile:
         reader= csv.DictReader(csvFile)
         for row in reader:
            self.senderGmail = row['SenderGmail']
            self.toList = row['ToList'].split(',')
            break
      #print(self.senderGmail, self.toList)
      
   def getMsg(self, textBody):
      return  '\r\n'.join(self.msgTemplateBase + 
                          [self.msgBlankText%textBody])

   def login(self):
      try:
         server = smtplib.SMTP_SSL('smtp.gmail.com')
      except:
         print('Failed to connect to server')
         raise
      while True:
         gmailPassword =  getpass.getpass('Input password or enter to cancel email notifications : ')
         if  gmailPassword == '':
            raise NoPasswordException
         try:
            server.login(self.senderGmail, gmailPassword)
            self.gmailPassword = gmailPassword
            return
         except KeyboardInterrupt:
            return
         except:
            print('login failed')
    
   def connectAndSend(self,outgoing):
      try:
         server = smtplib.SMTP_SSL('smtp.gmail.com')
      except:
         print('Failed to connect to server')
         return
      try:
         server.login(self.senderGmail, self.gmailPassword)
      except:
            print('login failed')
            return
      try:
         server.sendmail(self.senderGmail,
                         self.toList,
                         self.getMsg(outgoing))
         print('Mail sent!')
      except:
         print('failed to send mail')
         
if __name__=='__main__':
   import sys
   if len(sys.argv)== 1:
      print('usage: at least one argument is needed to constitute the outgoing message')
      sys.exit()
   try:
      mailer= AEMMailer()
      mailer.connectAndSend(' '.join(sys.argv[1:]))
   except NoPasswordException:
      print("If you don't supply a password, then no mail will be sent...")



        
