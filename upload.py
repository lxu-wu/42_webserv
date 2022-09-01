<<<<<<< HEAD
# https://www.tutorialspoint.com/How-do-we-do-a-file-upload-using-Python-CGI-Programming

#!/usr/bin/python
import cgi, os
import cgitb; cgitb.enable()
form = cgi.FieldStorage()
# Get filename here.
fileitem = form['filename']
# Test if the file was uploaded
if fileitem.filename:
   # strip leading path from file name to avoid
   # directory traversal attacks
   fn = os.path.basename(fileitem.filename)
   open('/tmp/' + fn, 'wb').write(fileitem.file.read())
   message = 'The file "' + fn + '" was uploaded successfully'
 
else:
   message = 'No file was uploaded'
 
print """\
Content-Type: text/html\n
<html>
<body>
   <p>%s</p>
</body>
</html>
""" % (message,)
=======
#!/usr/bin/python

# import os

# a = os.getenv('AUTH_TYPE')

# print "Content-type:text/html\r\n\r\n"
# print '<html>'
# print '<head>'
# print '<title>Hello World - First CGI Program</title>'
# print '</head>'
# print '<body>'
# print '<h2>Hello World! This'
# print a
# print 'is my first CGI program</h2>'
# print '</body>'
# print '</html>'

import cgi, os

form = cgi.FieldStorage()

fileitem = form['file1']

path = "./dossier"

isExist = os.path.exists(path)

if not isExist:
	os.makedirs(path)

if fileitem.filename:
	fn = os.path.basename(fileitem.filename)
	open(path + fn, 'wb').write(fileitem.file.read())
	message = "The file '" + fn + "' was uploaded successfully with python"

else:
	message = "No file was uploaded"

print """\
<html><body>
<p>%s</p>
</body></html>
""" % (message,)

# print ("<html>")
# print ("<head>")
# print ("<title>CGI xana</title>")
# print ("</head>")
# print ("<body>")
# print ("<h2>Hello</h2>")
# print ("</body>")
# print ("</html>")

# import sys
 
# for line in sys.stdin:
#     if 'q' == line.rstrip():
#         break
#     print(f'Input : {line}')
 
# print("Exit")


>>>>>>> ae9b615eda77ce63daf5b64a6e31a0c61acb9bdb
