#!/usr/bin/php
<?php
// Set content type header
echo "Content-Type: text/html\r\n\r\n";

$head = <<<END
	<!DOCTYPE html>
	<html lang="en">
	<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="style.css">
	<title>Simple Message App</title>

	</head>
	END;


$form= <<<END
	<form action="/notes/notes.php" method="POST" enctype="application/x-www-form-urlencoded">
	<input type="text" class="form-control" name="msg" placeholder="Enter your note" required  >
	<button>Submit Note</button>
	</form>
	<div class="title">
		<a href="/index.html" class="home-button">go to home</a>
	</div>
	 <script>
        // Faire défiler .applist vers le bas à chaque chargement de la page
        window.onload = function() {
            const applist = document.getElementById('applist');
            applist.scrollTop = applist.scrollHeight;
        };
    </script>
END;


$script= <<<END

END;





if( $_SERVER['REQUEST_METHOD'] == 'POST')
{
	$postData = array();
	//recuperer le contenu du body
	$rawPostData = file_get_contents("php://stdin");
	parse_str($rawPostData, $postData);
	$msg = 	nl2br((htmlspecialchars($postData['msg'], ENT_QUOTES, 'UTF-8')));

	//Ajouter la ligne a la fin du fichier
	$myfile = file_put_contents('html-files/notes/notes.txt', $msg.PHP_EOL , FILE_APPEND | LOCK_EX);
}
$lines = array();
//ouvrir le fichier et le parser => mettre chaque ligne dans un tableau
if (file_exists('html-files/notes/notes.txt')) {
	$lines = file('html-files/notes/notes.txt', FILE_SKIP_EMPTY_LINES);
}
else
{
	echo 'fichier pas trouve';
	$lines = array();
}
// Afficher la liste des notes
echo $head;
echo '<body><div class="app"><h1>Note taking app</h1><div class="applist" id="applist">';

$i = 1;
foreach($lines as $line)
{
	$line = nl2br($line);

	echo '<div class="appitem">';
	echo "<div class='index'><h4>$i</h4></div>";
	echo "<div class=note-content><code>$line</code></div>";
	echo "</div>\n";
	$i = $i +1;
}
echo '</div>';

echo $form;
echo '</div></body></html>';
exit;
?>
