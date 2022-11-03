function switchLanguage(lang) {
    let data = new FormData()
    data.append("lang", lang)
    fetch("/switch_lang", {
        "method": "POST",
        "body": data,
    }).then(response => {
        if (response.status === 200) {
            return response.json();
        } else {
            // handle this somehow
        }
    }).then(json => {
        let rb = document.getElementById("rb_" + lang)
        rb.checked = true;
        loadControlPanel(json)
        g_lang = lang
        console.log('Success! ')
    }).catch(error => {
        console.log('error with access token req!')
    })
}

function loadControlPanel(jsonResponse) {
    let CPanel = document.getElementById("ControlPanel")

    if (!CPanel) return
    //CPanel.innerHTML = ""
    let resTxt = ""
    for (let jsonResponseElement of jsonResponse.btns) {
        resTxt += jsonResponseElement.html
    }
    CPanel.innerHTML = resTxt;

    for (let jsonResponseElement of jsonResponse.btns) {
        if (jsonResponseElement.auth) {
            disableOption(jsonResponseElement, jsonResponse.auth_msg);
        }
    }
}

function disableOption(elem, auth_msg) {
    let maskTxt = "<div class=\"mask\" style=\"background-color: rgba(0, 0, 0, 0.6)\">\n" +
        "    <div class=\"d-flex justify-content-center align-items-center h-100\">\n" +
        `<p class=\"text-white mb-0\">${auth_msg}</p>` +
        "    </div>\n" +
        "  </div>"
    let container = document.getElementById(elem.id);
    let a_elem = document.getElementById("a_" + elem.id);
    a_elem.classList.add("disabled");
    container.innerHTML += maskTxt;
}