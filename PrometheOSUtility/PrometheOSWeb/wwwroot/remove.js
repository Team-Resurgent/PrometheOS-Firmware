window.onload = function () {
    document.getElementById("year").textContent = new Date().getFullYear();
    getBankInfo();
};

async function getBankInfo()
{
    await fetch("/api/bankinfo.json").then(async response => {

        const json = await response.json();

        let contentBody = "";
        for (let i = 0; i < json.length; i++) {
            if (json[i].slots > 0 && json[i].category == 0) {
                contentBody += "<p><div class=\"grid\"><a role=\"button\" href=\"javascript:void(0)\" onclick=\"removeBank(" + json[i].id + ")\">" + json[i].name + "</a></div></p>";
            }
        }
        let content = document.getElementById("content");
        content.innerHTML = contentBody == "" ? "No items found." : contentBody;

    }).catch(error => {

        let content = document.getElementById("content");
        content.innerHTML = "Failed to connect."

    });
}

async function removeBank(id)
{
    await fetch("/api/removebank?" + id).then(() => {
        getBankInfo();
    }).catch(error => {
        content.innerHTML = "Failed to connect."
    });
}
