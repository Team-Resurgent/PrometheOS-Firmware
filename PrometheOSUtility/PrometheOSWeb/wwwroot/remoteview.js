window.onload = function () {
    screenshot();
};
async function screenshot() {
    await fetch("http://192.168.1.66/api/screenshot").catch(error => {
        content.innerHTML = "Failed to connect."
    }).then(response => response.blob()).then(blob => {
        var url = window.URL.createObjectURL(blob);

        var img = document.createElement('img');
        img.src = url;
        img.alt = "Screenshot";
        img.style = "max-width: 100%; height: auto";

        const content = document.getElementById('content');
        content.innerHTML = "";
        content.appendChild(img);
    });
}