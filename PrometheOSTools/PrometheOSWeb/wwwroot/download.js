window.onload = function () {
    getBankInfo();
};

async function getBankInfo() {
    await fetch("http://192.168.1.151/api/bankinfo.json").then(async response => {

        const json = await response.json();

        let contentBody = "";
        for (let i = 0; i < json.length; i++) {
            if (json[i].slots > 0) {
                contentBody += "<p><div class=\"grid\"><a role=\"button\" href=\"#\" onclick=\"downloadBank(" + json[i].id + ", '" + json[i].name + ".bin')\">" + json[i].name + "</a></div></p>";
            }
        }
        let content = document.getElementById("content");
        content.innerHTML = contentBody == "" ? "No items found." : contentBody;
    }).catch(error => {
        let content = document.getElementById("content");
        content.innerHTML = "Failed to connect."
    });
}

async function downloadBank(id, name)
{
    await fetch("http://192.168.1.151/api/downloadbank?" + id).catch(error => {
        content.innerHTML = "Failed to connect."
    }).then(response => response.blob()).then(blob => {
        var url = window.URL.createObjectURL(blob);
        var a = document.createElement('a');
        a.href = url;
        a.download = name;
        document.body.appendChild(a); 
        a.click();
        a.remove(); 
    });
}