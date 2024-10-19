#!/usr/bin/env python3
import os
import sys

def handle_get():
    print("<html><body>")
    print("<h1>File Upload</h1>")
    print('<form enctype="multipart/form-data" action="/cgi-bin/script.py" method="POST">')
    print('<input type="file" name="uploaded_file"><br>')
    print('<input type="submit" value="Upload">')
    print('</form>')

    # If a file is uploaded, display the delete button
    if os.path.exists("uploads/uploaded_file.txt"):
        print("<h2>Uploaded File: uploaded_file.txt</h2>")
        print('<form action="/cgi-bin/script.py" method="POST">')
        print('<input type="hidden" name="_method" value="DELETE">')  # HTML doesn't support DELETE natively, use POST with a hidden field
        print('<input type="submit" value="Delete File">')
        print('</form>')

    print("</body></html>")



def handle_post():
    # Get the uploaded file data
    content_length = int(os.getenv('CONTENT_LENGTH', 0))
    post_data = sys.stdin.read(content_length)

    # Save the file (this example just saves the raw POST data)
    with open("uploads/uploaded_file.txt", "w") as f:
        f.write(post_data)

    print("Content-Type: text/html; charset=utf-8\n")
    print("<html><body>")
    print("<h1>File Successfully Uploaded</h1>")
    print('<form action="/cgi-bin/script.py" method="POST">')
    print('<input type="hidden" name="_method" value="DELETE">')  # Again, for DELETE method simulation
    print('<input type="submit" value="Delete File">')
    print('</form>')
    print("</body></html>")



def handle_delete():
    file_path = "uploads/uploaded_file.txt"
    if os.path.exists(file_path):
        os.remove(file_path)
        print("Content-Type: text/html\n")
        print("<html><body>")
        print(f"<h1>Dosya {file_path} başarıyla silindi.</h1>")
        print("</body></html>")
    else:
        print("Content-Type: text/html\n")
        print("<html><body>")
        print(f"<h1>Dosya {file_path} bulunamadı.</h1>")
        print("</body></html>")

def main():
    method = os.getenv("REQUEST_METHOD")
    if method == "GET":
        handle_get()
    elif method == "POST":
        # Form aracılığıyla _method parametresine bak, DELETE mi kontrol et
        if os.getenv("CONTENT_TYPE", "").startswith("multipart/form-data"):
            handle_post()
        elif os.getenv("CONTENT_TYPE", "").startswith("application/x-www-form-urlencoded"):
            post_data = sys.stdin.read(int(os.getenv('CONTENT_LENGTH', 0)))
            if "_method=DELETE" in post_data:
                handle_delete()
            else:
                handle_post()
    else:
        print("Content-Type: text/html\n")
        print("<html><body>")
        print(f"<h1>Unsupported Method: {method}</h1>")
        print("</body></html>")

if __name__ == "__main__":
    main()
