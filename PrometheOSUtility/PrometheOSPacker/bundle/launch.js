window.onload = function () {
    getBankInfo();
};

async function getBankInfo() {
    await fetch("http://192.168.1.151/api/bankinfo.json").then(async response => {

        const json = await response.json();

        let contentBody = "";
        for (let i = 0; i < json.length; i++) {
            if (json[i].slots > 0) {
                contentBody += "<p><div class=\"grid\"><a role=\"button\" href=\"#\" onclick=\"launchBank(" + json[i].id + ")\">" + json[i].name + "</a></div></p>";
            }
        }
        contentBody += "<p><div class=\"grid\"><a role=\"button\" href=\"#\" onclick=\"launchTsop()\">TSOP</a></div></p>";

        let content = document.getElementById("content");
        content.innerHTML = contentBody;
    }).catch(error => {
        let content = document.getElementById("content");
        content.innerHTML = "Failed to connect."
    });
}

async function launchBank(id)
{
    await fetch("http://192.168.1.151/api/launchbank?" + id).catch(error => {
        content.innerHTML = "Failed to connect."
    }).then(response => {
        if (response.status == 200) {
            let content = document.getElementById("content");
            content.innerHTML = "Launching..."
        } else {
            let content = document.getElementById("content");
            content.innerHTML = "Failed to launch."
        }
    });
}

async function launchTsop() {
    await fetch("http://192.168.1.151/api/launchtsop").catch(error => {
        content.innerHTML = "Failed to connect."
    }).then(response => {
        if (response.status == 200) {
            let content = document.getElementById("content");
            content.innerHTML = "Launching..."
        } else {
            let content = document.getElementById("content");
            content.innerHTML = "Failed to launch."
        }
    });
}