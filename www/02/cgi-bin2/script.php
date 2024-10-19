<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dosya Yukleme ve Silme Testi</title>
</head>
<body>
    <h1>Dosya Yukleme</h1>
    <!-- Dosya Yukleme Formu -->
    <form action="" method="POST" enctype="multipart/form-data" style="display: inline-block;">
        <input type="file" name="file" required>
        <button type="submit" name="upload">Dosyayi Yukle</button>
    </form>

    <?php
    $upload_dir = __DIR__ . '/../uploads';  // Yuklenen dosyalarin kaydedilecegi dizin
    // Dizin olusturma kodu kaldirildi

    // Mesajlari tutmak icin bir degisken
    $message = '';

    // Eger dosya yuklenmisse
    if (isset($_POST['upload']) && isset($_FILES['file'])) {
        $file_name = basename($_FILES['file']['name']);
        $file_path = $upload_dir . '/' . $file_name;

        // Dosyayi yukle
        if (move_uploaded_file($_FILES['file']['tmp_name'], $file_path)) {
            $message = "<h2>Dosya '$file_name' basariyla yüklendi!</h2>";
        } else {
            $message = "<h2>Dosya yuklenemedi!</h2>";
        }
    }

    // Eger dosya silme islemi yapilacaksa
    elseif (isset($_POST['delete']) && isset($_POST['filename'])) {
        $file_name = basename($_POST['filename']);
        $file_path = $upload_dir . '/' . $file_name;

        // Dosyayi sil
        if (file_exists($file_path)) {
            unlink($file_path);
            $message = "<h2>Dosya '$file_name' silindi.</h2>";
        } else {
            $message = "<h2>Dosya bulunamadi!</h2>";
        }
    }

    // Mesaji goster
    echo $message;
    ?>

    <!-- Dosyayi Silme Formu -->
    <?php if (isset($file_name) && file_exists($file_path)) : ?>
        <form action="" method="POST" style="display: inline-block; margin-left: 10px;">
            <input type="hidden" name="filename" value="<?php echo htmlspecialchars($file_name); ?>">
            <button type="submit" name="delete">Dosyayi Sil</button>
        </form>
    <?php endif; ?>

    <h2>Yuklu Dosyalar</h2>
    <ul>
    <?php
    // Yuklu dosyalari listeleyin
    $files = scandir($upload_dir);
    foreach ($files as $file) {
        if ($file !== '.' && $file !== '..') {
            echo "<li>$file
                    <form action='' method='POST' style='display: inline;'>
                        <input type='hidden' name='filename' value='" . htmlspecialchars($file) . "'>
                        <button type='submit' name='delete'>Sil</button>
                    </form>
                  </li>";
        }
    }
    ?>
    </ul>

</body>
</html>
