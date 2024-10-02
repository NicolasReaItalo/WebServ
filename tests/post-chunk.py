import requests


def generate_chunks(file_path, chunk_size=1024):
    """Générateur pour lire le fichier en petits morceaux (chunks)."""
    with open(file_path, 'rb') as f:
        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break
            yield chunk

# URL de destination
url = 'http://localhost:8080/upload/test.jpg'

# Chemin du fichier à envoyer
file_path = '/home/nrea/Documents/PROJETS/webserv/github/tests/noise.jpg'

# Requête POST avec transfert chunked
with requests.post(url, data=generate_chunks(file_path), headers={'Transfer-Encoding': 'chunked'}, stream=True) as response:
    # Vérifier la réponse du serveur
    print(f"Statut de la réponse : {response.status_code}")
    print(f"Contenu de la réponse : {response.text}")
