import requests

# Faire une requête GET
url = 'http://localhost:8080/index.html'

# Réponse avec stream=True pour lire les données en morceaux
with requests.get(url, stream=True) as response:
	if response.headers.get('Transfer-Encoding') == 'chunked':
		print("La réponse est chunked")

	print(f"Contenu de la réponse : {response.text}")
	print(f"headers : {response.headers}")
	# Lire les données chunk par chunk
	for chunk in response.iter_content(chunk_size=100):
		if chunk:
			print(f"Chunk reçu de taille {len(chunk)} octets")
