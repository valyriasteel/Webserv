#!/usr/bin/env python3
import os
import sys
import cgi

UPLOAD_DIR = os.path.join(os.path.dirname(__file__), "../uploads")

# Dosya yükleme formunu ve mesajı aynı sayfada gösterme fonksiyonu
def render_page(message=""):
    print("<html><body>")
    print("<h1>File Upload</h1>")
    
    # Dosya yükleme formu
    print('<form enctype="multipart/form-data" action="/cgi-bin/script.py" method="POST">')
    print('<input type="file" name="uploaded_file"><br>')
    print('<input type="submit" value="Upload">')
    print('</form>')
    
    # Yükleme sonrası mesajı göster
    if message:
        print(f"<p>{message}</p>")
    
    # Yüklü dosyaları listele ve silme butonları oluştur
    if os.path.exists(UPLOAD_DIR):
        print("<h2>Uploaded Files:</h2>")
        for filename in os.listdir(UPLOAD_DIR):
            file_path = os.path.join(UPLOAD_DIR, filename)
            if os.path.isfile(file_path):
                print(f"<p>{filename} <form action='/cgi-bin/script.py' method='POST' style='display:inline;'>")
                print(f"<input type='hidden' name='delete_file' value='{filename}'>")
                print(f"<input type='submit' value='Delete'></form></p>")
    print("</body></html>")

# Dosya yükleme işlemi
def handle_post():
    form = cgi.FieldStorage()
    fileitem = form['uploaded_file']

    if fileitem.filename:
        # Dizini kontrol et, ancak oluşturmadan devam et
        if not os.path.exists(UPLOAD_DIR):
            render_page("Upload directory does not exist.")
            return

        # Dosya kaydetme
        filename = os.path.basename(fileitem.filename)
        filepath = os.path.join(UPLOAD_DIR, filename)
        with open(filepath, 'wb') as f:
            f.write(fileitem.file.read())

        message = f"File {filename} Successfully Uploaded."
        render_page(message)
    else:
        render_page("No file was uploaded.")

# Dosya silme işlemi
def handle_delete():
    form = cgi.FieldStorage()
    filename = form.getvalue('delete_file')
    
    if filename:
        filepath = os.path.join(UPLOAD_DIR, filename)
        if os.path.exists(filepath):
            os.remove(filepath)
            render_page(f"File {filename} Successfully Deleted.")
        else:
            render_page(f"File {filename} not found.")
    else:
        render_page("No file selected for deletion.")

def main():
    method = os.getenv("REQUEST_METHOD")
    if method == "POST":
        content_type = os.getenv("CONTENT_TYPE", "")
        if "multipart/form-data" in content_type:
            handle_post()
        elif "application/x-www-form-urlencoded" in content_type:
            handle_delete()  # Form tipi doğruysa dosya silme fonksiyonu çağrılır
        else:
            print("Content-Type: text/html\n")
            print("<html><body>")
            print(f"<h1>Bad Request: Unsupported Content-Type</h1>")
            print(f"<p>{content_type}</p>")
            print("</body></html>")
    else:
        render_page()

if __name__ == "__main__":
    main()
