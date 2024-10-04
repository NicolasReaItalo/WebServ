#!/usr/bin/php-cgi
<?php
// Obligatoire en CGI : envoyer l'en-tête Content-Type avant tout contenu
header("Content-Type: text/html");

// Récupération des variables CGI
$method = $_SERVER['REQUEST_METHOD'];
$query_string = $_SERVER['QUERY_STRING'];

$fname = $_GET["first_name"];
$lname = $_GET["last_name"];

// Affichage d'une réponse HTML basique
echo "<html>";
echo "<head><title>CGI PHP Example</title></head>";
echo "<body>";
echo "<h1>Exemple de Script CGI en PHP</h1>\n";
echo "<p>Méthode utilisée : $method</p>\n";
echo "<p>Query string : $query_string</p>\n";
echo "<p>First name : $fname</p>\n";
echo "<p>Last name : $lname</p>\n";
print_r($_GET);
echo "</body>";
echo "</html>";
?>
