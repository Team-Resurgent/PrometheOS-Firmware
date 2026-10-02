window.onload = function () {
    document.getElementById("year").textContent = new Date().getFullYear();
};

async function downloadEeprom() {
    await fetch("/api/downloadeeprom").catch(error => {
        content.innerHTML = "Failed to connect."
    }).then(response => response.blob()).then(blob => {
        var url = window.URL.createObjectURL(blob);
        var a = document.createElement('a');
        a.href = url;
        a.download = "EEPROM.bin";
        document.body.appendChild(a);
        a.click();
        a.remove();
    });
}

async function downloadPrometheOS(id, name) {
    await fetch("/api/downloadprom").catch(error => {
        content.innerHTML = "Failed to connect."
    }).then(response => response.blob()).then(blob => {
        var url = window.URL.createObjectURL(blob);
        var a = document.createElement('a');
        a.href = url;
        a.download = "PrometheOS.bin";
        document.body.appendChild(a);
        a.click();
        a.remove();
    });
}
