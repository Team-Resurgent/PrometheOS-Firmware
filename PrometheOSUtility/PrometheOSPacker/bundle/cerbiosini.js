window.onload = function () {
    getCerbiosIni();
};

async function getCerbiosIni()
{
    await fetch("http://192.168.1.66/api/cerbiosini").then(async response => {

        const reader = response.body.getReader();
        const decoder = new TextDecoder("utf-8");

        let body = '';

        while (true)
        {
            const { done, value } = await reader.read();
            if (done)
            {
                break;
            }
            const chunk = decoder.decode(value, { stream: true });
            body += chunk;
        }

        let contentBody = "";
        contentBody += "<p><div class=\"grid\"><textarea rows=\"40\" cols=\"80\" spellcheck=\"false\" id=\"cerbiosini\">" + body + "</textarea></div></p>";
        let content = document.getElementById("content");
        content.innerHTML = contentBody;

    }).catch(error => {

        let content = document.getElementById("content");
        content.innerHTML = "Failed to connect."

    });
}

async function setCerbiosIni()
{
    let content = document.getElementById("cerbiosini");

    var data = new FormData();
    data.append('body', content.innerText);
    await fetch("http://192.168.1.66/api/cerbiosini", {
        method: 'POST',
        body: data
    }).then(response => {
        if (response.status == 200) {
            window.location.href = "/index.html";
        } else {
            uploadFailure();
        }
    }).catch(() => {
        uploadFailure()
    }
    );
}

function uploadFailure() {
    alert("Upload failed.");
}
