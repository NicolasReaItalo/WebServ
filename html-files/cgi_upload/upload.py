#!/usr/bin/env python

import cgi, os
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

	<title>File Upload</title>
</head>
<body>
	<div class="app">
		<div class="header">
			<h1>FILE UPLOAD</h1>
		</div>

		<h3>Files uploaded</h3>
		<div class="uploaded-list">
""")

# Fichiers uploade eventuels
if os.environ['REQUEST_METHOD'] == 'POST':
	form = cgi.FieldStorage()

	if 'files[]' in form:
		files = form['files[]']
		if not isinstance(files,list):
			files = [files]
		for file in files:
			if file.filename:
				file_name = file.filename
				file_path = 'html-files/cgi_upload/uploaded/' + file_name
				with open(file_path, 'wb') as f:
					while True:
						chunk = file.file.read(1024)
						if not chunk:
							break
						f.write(chunk)
				print(f"""
				<div class="uploaded-file">
				<div><img class="file-icon" src="file.png" alt="">{file_name} uploaded succesfully</div>
				</div>""")

print("""
			</div>
			<h3>Choose files to upload</h3>
		<div class="upload-form">
			<form id="uploadForm" action="/cgi_upload/upload.py" method="POST" enctype="multipart/form-data">
				<div id="fileList">
					<div class="file-input">
						<input type="file" name="files[]">
					</div>
				</div>

				<button type="button" onclick="addFileInput()">Add another file</button>
				<button type="submit">Send files</button>
			</form>
		</div>
	  <br>
	  <div class="home-conf">
	  	<div class="home-button">
			<a href="/index.html" class="home-button">go to home</a>
		</div>
	  </div>
	  
	  
	</div>
	  
</div>

<script>
	function addFileInput() {
		const fileList = document.getElementById('fileList');
		const newInput = document.createElement('div');
		newInput.classList.add('file-input');
		newInput.innerHTML = '<input type="file" name="files[]">';
		fileList.appendChild(newInput);
	}
</script>

</body>
</html>\r\n\r\n""")

exit()

