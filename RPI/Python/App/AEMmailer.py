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
      """ 
      create an instance by loading account info from local csv file
      then login
      the set constant fields subject and template
      """
      self.initToListAndSender()
      if not self.toList or self.senderGmail == '':
         raise MissingEmailAccountsException
      self.login()
      self.subject     = 'AEM Monitor'   ##<! subject line for emails
      self.msgBlankText     =  '%s'       ##<! template for email body text

   def setSender(self,csvFile):
      """
      read the self.SenderGmail from the csvFile
      @param csvFile an open csv file ready for reading
      """
      reader= csv.DictReader(csvFile)
      self.senderGmail = next(reader)['SenderGmail']

   def setToList(self,csvFile):
      """
      read the self.toList elts from the csvFile
      @param csvFile an open csv file ready for reading
      """
      reader= csv.DictReader(csvFile)
      self.toList = []
      for row in reader:
         self.toList += [row['ToList']]

   def initToListAndSender(self, sender=True, toLis=True, filenName='./mail.csv'):
      """
      read the csv file and obtain from it senderGmail and toList
      @param sender if True the sender is read from file
      @param toLis if True the toLis is read from file
      @param fileName the csv file containing 1 row of headers and 1 row of data
      """
      if sender:
         with open(filenName, newline='') as csvFile:
            self.setSender(csvFile)
      if toLis:
         with open(filenName, newline='') as csvFile:
            self.setToList(csvFile)
            
   def getMsg(self, textBody):
      """
      returns a fully formatted email based on string argument
      @param textBody a string that will be formatted into the email
      @return the fully formatted email
      """
      msgTemplateBase = ['To: %s ' % ", ".join(self.toList),   ##<! template for email headers
                         'From: %s' % self.senderGmail,
                         'Subject: %s' % self.subject,
                         '',]
      return  '\r\n'.join(msgTemplateBase + 
                          [self.msgBlankText%textBody])

   def login(self):
      """
      try to connect to server, in case of failure, raise and exit
      get a hush password from user, try to login, if no password entered, raise NoPassword Exception
      if password, try to login, if failure, loop,
      if success, ave password for future use as self.gmailPassword
      """
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
      """
      try to connect to server, login, and send email after formatting outgoing arg.
      all failure are handled and do not stop exectuion,
      print to stdout if mail is sent
      @param outgoing :  a string to be formatted as the email body tex
      """
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
         self.initToListAndSender(sender=False)
         server.sendmail(self.senderGmail,
                         self.toList,
                         self.getMsg(outgoing))
         print('Mail sent!')
      except:
         print('Failed to send mail')

def runTest(args=None,iterate=False):
   import time
   try:
      mailer= AEMMailer()
   except NoPasswordException:
      print("If you don't supply a password, then no mail will be sent...")
      return

   outgoing = '\nMail count %d'
   if args:
      outgoing = args + outgoing
   count= 0
   mailer.subject = 'TEST: ' + mailer.subject
   mailer.connectAndSend(outgoing% count)
   if iterate:
      while True:
         try:
            time.sleep(20)
            count+=1
            mailer.connectAndSend(outgoing% count)
         except KeyboardInterrupt:
            print('\nTest complete!')
            return

if __name__=='__main__':
   import sys
   if len(sys.argv)== 1:
      print('usage: at least one argument is needed to constitute the outgoing message')
      sys.exit()
   body = ' '.join(sys.argv[1:])
   runTest(args=body,iterate=True)




        
