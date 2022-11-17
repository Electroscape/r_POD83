/*  Takes different actions based on buttons pushed and checks for the correct
 *  length and password entered
 */

// Required password
const password = [1, 2, 3, 4];
// What the user has entered
var entered = [];

/* Checks to see if the entered password is the correct length
 * and if each button pushed matches the password
 */
function isCorrect() {
    let correctPass = true;
    if (password.length !== entered.length) {
        correctPass = false;
    }

    for (let i = 0; i < password.length; i++) {
        if (entered[i] !== password[i]) {
            correctPass = false;
        }
    }

    if (!correctPass) {
        keypadClear();
    }
    return correctPass;
}

function keypadClear() {
    // Gets the element IDs of the screen, hash button, and asterisk button
    const screen = document.getElementById('screen');
    // Resets the screen to blank, and resets the entered array to empty
    screen.textContent = '';
    screen.classList.remove('failed');
    screen.classList.remove('success');
    entered.length = 0;
}

/* Event Listener on page load
 */
window.addEventListener('load', function () {
    // Gets the element IDs of the screen, hash button, and asterisk button
    const screen = document.getElementById('screen');
    const hash = document.getElementById('button-hash');
    const ast = document.getElementById('button-ast');

    // Event listener for when # is clicked
    // This should be the last button clicked
    hash.addEventListener('click', function () {
        // Adds # to the screen and pushes to the entered array
        //screen.textContent += '#';
        //entered.push('#');

        // See if entered password matches, adds the CSS success tag,
        // and changes the screen message to "success"
        if (!entered.length) {
            return false;
        } else if (isCorrect()) {
            screen.classList.add('success');
            screen.textContent = 'success';
        } else {
            screen.classList.add('failed');
            screen.textContent = 'wrong';
        }

        // clear result message
        setTimeout(function () {
            keypadClear();
        }, 1000);
    });

    // Event listener for when * is clicked
    ast.addEventListener('click', function () {
        keypadClear();
    });

    // Helper loop for the bind() function to keep hold of the iterator
    // For buttons 0-9
    // Used http://stackoverflow.com/questions/8909652/adding-click-event-listeners-in-loop for the following
    for (let t = 0; t < 10; t++) {
        bind(t);
    }

    // Helper function to create and keep each instance of the buttons
    // Used for buttons 0-9
    function bind(t) {
        let button = document.getElementById('button-' + t);
        button.addEventListener('click', function () {
            // Adds the number to the screen and pushes it to the entered array
            screen.textContent += t;
            entered.push(t);
        });
    }
});