function ValidateIPaddress() {
    let inputText = document.getElementById("ipv4");

    let ipFormat = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
    if (inputText.value.match(ipFormat)) {
        alert("CORRECT! This is a valid IP :D");
    } else {
        alert("WRONG! You have entered an invalid IP address!");
    }
    inputText.value = "";
    return false;
}