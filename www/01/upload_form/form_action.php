<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Submitted Information</title>
</head>
<body>
    <h1>Submitted Information</h1>

    <?php
    // Process the form if it was submitted
    if ($_SERVER['REQUEST_METHOD'] == 'POST') {
        // Sanitize incoming POST data
        $first_name = htmlspecialchars($_POST['first_name']);
        $last_name = htmlspecialchars($_POST['last_name']);
        $username = htmlspecialchars($_POST['username']);
        $message = htmlspecialchars($_POST['message']);

        // Display the sanitized data
        echo "<p><strong>First Name:</strong> $first_name</p>";
        echo "<p><strong>Last Name:</strong> $last_name</p>";
        echo "<p><strong>Username:</strong> $username</p>";
        echo "<p><strong>Message:</strong> $message</p>";
    } else {
        // Display a message if the form was not submitted
        echo "<p>No form was submitted.</p>";
    }
    ?>
</body>
</html>