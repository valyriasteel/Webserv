<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <title>Gönderilen Bilgiler</title>
</head>
<body>
    <h1>Gönderilen Bilgiler</h1>

    <?php
    // Enable error reporting
    error_reporting(E_ALL);
    ini_set('display_errors', 1);

    // Debugging: Display environment variables
    echo '<pre>';
    echo '$_SERVER[\'REQUEST_METHOD\']: ' . $_SERVER['REQUEST_METHOD'] . "\n";
    echo '$_SERVER[\'CONTENT_TYPE\']: ' . $_SERVER['CONTENT_TYPE'] . "\n";
    echo '$_SERVER[\'CONTENT_LENGTH\']: ' . $_SERVER['CONTENT_LENGTH'] . "\n";
    echo '</pre>';

    // Form gönderildiyse verileri göster
    if ($_SERVER['REQUEST_METHOD'] == 'POST') {
        echo '<pre>';
        echo '$_POST: ';
        print_r($_POST);  // Tüm POST verilerini ekrana basar
        echo 'php://input: ';
        echo file_get_contents('php://input');
        echo '</pre>';

        $isim = htmlspecialchars($_POST['isim']);
        $soyisim = htmlspecialchars($_POST['soyisim']);
        $kullanici_adi = htmlspecialchars($_POST['kullanici_adi']);
        $mesaj = htmlspecialchars($_POST['mesaj']);

        echo "<p><strong>İsim:</strong> $isim</p>";
        echo "<p><strong>Soyisim:</strong> $soyisim</p>";
        echo "<p><strong>Kullanıcı Adı:</strong> $kullanici_adi</p>";
        echo "<p><strong>Mesaj:</strong> $mesaj</p>";
    }
    else {
        echo "<p>Form gönderilmedi.</p>";
    }
    ?>
</body>
</html>
