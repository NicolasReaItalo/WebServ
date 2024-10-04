#!/usr/bin/php-cgi
<?php
// Obligatoire en CGI : envoyer l'en-tête Content-Type avant tout contenu
header("Content-Type: text/html");

// Récupération des variables CGI
$method = $_SERVER['REQUEST_METHOD'];
$query_string = $_SERVER['QUERY_STRING'];

parse_str($query_string, $output);

// Affichage d'une réponse HTML basique
echo "<html>";
echo "<head><title>CGI PHP Example</title></head>";
echo "<body>";
echo "<h1>Exemple de Script CGI en PHP</h1>\n";
echo "<p>Méthode utilisée : $method</p>\n";
echo "<p>Query string : $query_string</p>\n";
echo "<p>First name :" . $output['first_name'] . "</p>\n";
echo "<p>Last name :" . $output['last_name'] . "</p>\n";
print_r($_GET);
echo "</body>";
echo "</html>";
?>
