window.onload = function () {
    document.getElementById("year").textContent = new Date().getFullYear();
    loadSystemInfo();
};

function displayValue(value) {
    if (value == null || value === "") {
        return "—";
    }
    return value;
}

function addSection(title, rows) {
    let html = "<article class=\"prom-info-card\"><h3>" + title + "</h3><table><tbody>";
    for (let i = 0; i < rows.length; i++) {
        html += "<tr><th>" + rows[i][0] + "</th><td>" + displayValue(rows[i][1]) + "</td></tr>";
    }
    html += "</tbody></table></article>";
    return html;
}

async function loadSystemInfo() {
    const content = document.getElementById("content");
    try {
        const response = await Promethe.api("systeminfo.json");
        if (!response.ok) {
            throw new Error("request failed");
        }

        const data = await response.json();
        let html = "";

        html += addSection("Console", [
            ["CPU", data.console.cpuMhz + " MHz"],
            ["GPU", data.console.gpuMhz + " MHz"],
            ["FSB", data.console.fsbMhz + " MHz"],
            ["RAM Clock", data.console.memMhz + " MHz"],
            ["RAM Size", data.console.ramMb + " MB"],
            ["Xbox Revision", data.console.xboxRev],
            ["Serial", data.console.serial],
            ["MAC", data.console.mac]
        ]);

        const storageRows = [
            ["Primary Drive", data.storage.primaryModel],
            ["Primary Serial", data.storage.primarySerial],
            ["Secondary Drive", data.storage.secondaryModel],
            ["Secondary Serial", data.storage.secondarySerial]
        ];
        html += addSection("Storage", storageRows);

        if (data.storage.partitions && data.storage.partitions.length > 0) {
            let partitionHtml = "<article class=\"prom-info-card\"><h3>Partitions</h3><table><tbody>";
            for (let i = 0; i < data.storage.partitions.length; i++) {
                const part = data.storage.partitions[i];
                const percent = part.total > 0
                    ? Math.round((part.used / part.total) * 100)
                    : 0;
                partitionHtml += "<tr><th>" + part.label + "</th><td>"
                    + Promethe.formatBytes(part.used) + " / " + Promethe.formatBytes(part.total)
                    + " (" + percent + "%)</td></tr>";
            }
            partitionHtml += "</tbody></table></article>";
            html += partitionHtml;
        }

        html += addSection("Audio", [
            ["Mode", data.audio.mode],
            ["AC3", data.audio.ac3 ? "Yes" : "No"],
            ["DTS", data.audio.dts ? "Yes" : "No"]
        ]);

        html += addSection("Video", [
            ["Standard", data.video.standard],
            ["Game Region", data.video.gameRegion],
            ["DVD Region", data.video.dvdRegion],
            ["AV Pack", data.video.avPack],
            ["Resolution", data.video.avRes],
            ["Encoder", data.video.encoder]
        ]);

        html += addSection("About", [
            ["PrometheOS", data.about.version],
            ["Build", data.about.build],
            ["Skin Author", data.about.skinAuthor],
            ["IP Address", data.about.ip]
        ]);

        content.innerHTML = html;
    } catch (error) {
        content.textContent = "Failed to connect.";
    }
}
