let ledColorState = 0;

window.onload = function () {

    document.getElementById("year").textContent = new Date().getFullYear();

    const fileDrop = document.getElementById("file-drop");
    fileDrop.addEventListener("dragover", (event) => {
        event.preventDefault();
        fileDrop.classList.add("dragover");
    });

    fileDrop.addEventListener("dragleave", () => {
        fileDrop.classList.remove("dragover");
    });

    fileDrop.addEventListener("drop", (event) => {
        event.preventDefault();
        fileDrop.classList.remove("dragover");
        const files = event.dataTransfer.files;
        if (files.length > 0) {
            const dataTransfer = new DataTransfer();
            dataTransfer.items.add(files[0]); 
            const fileInput = document.getElementById("file");
            fileInput.files = dataTransfer.files;
            refreshFileInfo();
        }
    });

    refreshFileInfo();
};

function getFileSize()
{
    const file = document.getElementById("file");
    if (file.value == "") {
        return -1;
    }

    const fileSize = file.files[0].size; 
    if (fileSize == 0) {
        return -1;
    } else if (fileSize % (256 * 1024) != 0) {
        return -1;
    } else if (fileSize > (1024 * 1024)) {
        return -1;
    }
    return fileSize;
}

async function refreshFileInfo()
{
    const file = document.getElementById("file");
    if (file.value != "") {
        const name = document.getElementById("name");
        name.value = file.files[0].name.substring(0, 40);
    }

    await fetch("/api/freeslots.json").then(async response => {

        const json = await response.json();
        const freeSlots = document.getElementById("freeSlots");
        freeSlots.textContent = json["freeslots"];

        const upload = document.getElementById("upload");
        const slotsNeeeded = document.getElementById("slotsNeeded");
        const fileSize = getFileSize();

        if (fileSize < 0) {
            upload.setAttribute("disabled", "disabled");
            slotsNeeeded.textContent = "NA";
        }
        else
        {
            const slotsNeededValue = fileSize / (256 * 1024);
            slotsNeeeded.textContent = slotsNeededValue;
            if (slotsNeededValue > json["freeslots"]) {
                upload.setAttribute("disabled", "disabled");
            } else {
                upload.removeAttribute("disabled");
            }
        }

    }).catch(error => {
        // 
    });
}

function setLedColor(ledColor)
{
    ledColorState = ledColor;

    let ledColorText = document.getElementById("ledColor");
    switch (ledColor) {
        case 1:
            ledColorText.textContent = "Red";
            break;
        case 2:
            ledColorText.textContent = "Green";
            break;
        case 3:
            ledColorText.textContent = "Yellow";
            break;
        case 4:
            ledColorText.textContent = "Blue";
            break;
        case 5:
            ledColorText.textContent = "Purple";
            break;
        case 6:
            ledColorText.textContent = "Turquoise";
            break;
        case 7:
            ledColorText.textContent = "White";
            break;
        default:
            ledColorText.textContent = "Off";
    }
}

async function upload()
{
    const upload = document.getElementById("upload");
    upload.setAttribute("disabled", "disabled");
    const file = document.getElementById("file");
    file.setAttribute("disabled", "disabled");

    const name = document.getElementById("name");

    var data = new FormData();
    data.append('file', file.files[0]);
    data.append('body', "{\"ledColor\":" + ledColorState + ",\"bankName\":\"" + name.value + "\"}");
    await fetch("/api/upload", {
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

function uploadFailure()
{
    alert("Upload failed.");
    let upload = document.getElementById("upload");
    upload.removeAttribute("disabled");
    let file = document.getElementById("file");
    file.removeAttribute("disabled");
}
