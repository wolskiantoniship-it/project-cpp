
import smtplib
import sys
from email.mime.text import MIMEText

def send_email(receiver_email, project_name):
    # --- KONFIGURACJA (Użyj hasła aplikacji Google!) ---
    sender = "wolski.antoni2005@gmail.com"
    password = ""

    subject = f"Nowe zlecenie: {project_name}"
    body = f"Witaj!\n\nZostałeś przydzielony do nowego projektu: {project_name}.\nZaloguj się do systemu, aby zobaczyć szczegóły."

    msg = MIMEText(body)
    msg['Subject'] = subject
    msg['From'] = sender
    msg['To'] = receiver_email

    try:
        with smtplib.SMTP_SSL('smtp.gmail.com', 465) as server:
            server.login(sender, password)
            server.sendmail(sender, receiver_email, msg.as_string())
        print("SUKCES")
    except Exception as e:
        print(f"ERROR: {e}")

if __name__ == "__main__":
    # Odbieranie argumentów z C++: sys.argv[1] to email, sys.argv[2] to nazwa projektu
    if len(sys.argv) > 2:
        send_email(sys.argv[1], sys.argv[2])



