async function downloadEeprom() {
    await fetch("http://192.168.1.66/api/downloadeeprom").catch(error => {
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
    await fetch("http://192.168.1.66/api/downloadprom").catch(error => {
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
