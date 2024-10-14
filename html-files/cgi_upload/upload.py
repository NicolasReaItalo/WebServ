#!/usr/bin/env python

import json, requests, cgi, os, datetime
import cgitb

cgitb.enable()

header = "Content-Type: text/html\r\n"
print(header,end='\r\n')
print ( """
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="style.css">
	<title>MEGAUPLOAD</title>
</head>
<body>
<div class='app'>
	<h1>MEGAUPLOAD</h1>
""")

# Fichiers uploade eventuels
print('<div class="uploaded-list">')
if os.environ['REQUEST_METHOD'] == 'POST':
	form = cgi.FieldStorage()

	if 'files[]' in form:
		files = form['files[]']
		if not isinstance(files,list):
			files = [files]
		for file in files:
			print(file)
			if file.filename:
				file_name = file.filename
				file_path = 'html-files/cgi_upload/uploaded/' + file_name
				with open(file_path, 'wb') as f:
					f.write(file.file.read())
				print('<div class="uploaded-file">')
				print(f"<h4>File '{file_name}' uploaded successfully.</h4>")
				upload_datetime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
				print(f"<div class='file-info'>Uploaded on: {upload_datetime}</div>")
				print('</div>')
print('</div>')
print('</div>')
print ("""</body>
</html>\r\n""")
exit()

