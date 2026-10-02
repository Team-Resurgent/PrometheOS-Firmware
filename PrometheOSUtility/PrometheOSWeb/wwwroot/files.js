let currentPath = "";
let drivesMounted = false;
let nameDialogResolve = null;

window.onload = function () {
    document.getElementById("year").textContent = new Date().getFullYear();
    initializeFileManager();

    // Delegated click handler for the file list (rebuilt on every navigation). Entries carry their
    // path/name in data- attributes rather than inline onclick, so any filename -- quotes, <, &,
    // backslashes -- is handled safely.
    document.getElementById("fileList").addEventListener("click", onFileListClick);

    // If the name dialog is dismissed (Escape / backdrop) without OK/Cancel, resolve its promise null.
    document.getElementById("nameDialog").addEventListener("close", function () {
        if (nameDialogResolve) {
            const resolve = nameDialogResolve;
            nameDialogResolve = null;
            resolve(null);
        }
    });
};

// Show the shared name dialog; resolves to the entered (trimmed) value on OK, or null on cancel.
function openNameDialog(title, defaultValue) {
    return new Promise(function (resolve) {
        nameDialogResolve = resolve;
        document.getElementById("nameDialogTitle").textContent = title;
        const input = document.getElementById("nameDialogInput");
        input.value = defaultValue || "";
        document.getElementById("nameDialog").showModal();
        input.focus();
        input.select();
    });
}

function closeNameDialog(accept) {
    const value = document.getElementById("nameDialogInput").value.trim();
    const resolve = nameDialogResolve;
    nameDialogResolve = null; // null first so the dialog's close handler won't double-resolve
    document.getElementById("nameDialog").close();
    if (resolve) {
        resolve(accept ? value : null);
    }
}

function isValidName(name) {
    if (name === null || name === "") {
        return false;
    }
    if (name.indexOf("\\") >= 0 || name.indexOf("/") >= 0) {
        Promethe.toast("Name cannot contain slashes.", "error");
        return false;
    }
    return true;
}

// HTML-escape for text content / attribute values (attribute also escapes quotes). getAttribute()
// decodes these back to the raw string, so the handlers receive the exact path/name.
function htmlText(s) {
    return String(s).replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
}
function htmlAttr(s) {
    return htmlText(s).replace(/"/g, "&quot;");
}

function onFileListClick(event) {
    const el = event.target.closest("[data-action]");
    if (!el) {
        return;
    }
    const action = el.getAttribute("data-action");
    const path = el.getAttribute("data-path");
    if (action === "open") {
        loadPath(path);
    } else if (action === "rename") {
        renameEntry(path, el.getAttribute("data-name"));
    } else if (action === "delete") {
        deleteEntry(path);
    }
}

async function waitForMountReady() {
    while (true) {
        const response = await Promethe.api("mountready.json");
        if (!response.ok) {
            throw new Error("Failed to check mount status.");
        }
        const data = await response.json();
        if (data.ready === true) {
            return;
        }
        await new Promise(function (resolve) {
            window.setTimeout(resolve, 100);
        });
    }
}

async function ensureDrivesMounted() {
    if (drivesMounted) {
        return;
    }

    const response = await Promethe.api("mountdrives");
    if (!response.ok) {
        throw new Error("Failed to request drive mount.");
    }

    await waitForMountReady();
    drivesMounted = true;
}

async function initializeFileManager() {
    const fileList = document.getElementById("fileList");
    fileList.textContent = "Mounting drives...";

    try {
        await ensureDrivesMounted();
        await loadPath("");
    } catch (error) {
        fileList.textContent = "Failed to connect.";
    }
}

function parentPath(path) {
    if (!path || path.length <= 3) {
        return "";
    }

    let trimmed = path;
    if (trimmed.endsWith("\\")) {
        trimmed = trimmed.slice(0, -1);
    }

    const slash = trimmed.lastIndexOf("\\");
    if (slash <= 2) {
        return trimmed.slice(0, slash + 1);
    }
    return trimmed.slice(0, slash + 1);
}

function pathSegments(path) {
    if (!path) {
        return [];
    }
    return path.split("\\").filter(function (part) {
        return part.length > 0;
    });
}

function pathForSegments(segments, count) {
    if (count <= 0) {
        return "";
    }
    const joined = segments.slice(0, count).join("\\");
    if (joined.charAt(joined.length - 1) === ":") {
        return joined + "\\";
    }
    return joined.endsWith("\\") ? joined : joined + "\\";
}

function renderPathLink(targetPath, label) {
    const escapedPath = targetPath.replace(/\\/g, "\\\\").replace(/'/g, "\\'");
    return "<a class=\"prom-file-dir\" href=\"javascript:void(0)\" onclick=\"loadPath('" + escapedPath + "')\">"
        + label + "</a>";
}

function renderPathBar(path) {
    const pathLabel = document.getElementById("currentPath");
    const upButton = document.getElementById("pathUp");
    if (pathLabel == null) {
        return;
    }

    if (!path) {
        pathLabel.textContent = "Root";
    } else {
        const segments = pathSegments(path);
        let html = renderPathLink("", "Root");
        for (let i = 0; i < segments.length; i++) {
            const targetPath = pathForSegments(segments, i + 1);
            const isLast = i === segments.length - 1;
            html += " <span class=\"prom-files-path-sep\">/</span> ";
            if (isLast) {
                html += "<span class=\"prom-files-path-current\">" + segments[i] + "</span>";
            } else {
                html += renderPathLink(targetPath, segments[i]);
            }
        }
        pathLabel.innerHTML = html;
    }

    if (upButton != null) {
        if (path) {
            upButton.hidden = false;
            upButton.onclick = function () {
                loadPath(parentPath(path));
            };
        } else {
            upButton.hidden = true;
            upButton.onclick = null;
        }
    }
}

function isNavigableEntry(entry) {
    return entry.dir === true || isDriveRoot(entry.path);
}

function entryIcon(entry) {
    return isNavigableEntry(entry) ? "\uD83D\uDCC1 " : "\uD83D\uDCC4 ";
}

function isDriveRoot(path) {
    if (!path) {
        return true;
    }
    if (/^SDCARD:\\?$/i.test(path)) {
        return true;
    }
    const colonIndex = path.indexOf(":");
    return colonIndex === path.length - 1;
}

function updateUploadBar() {
    const uploadBar = document.getElementById("uploadBar");
    if (uploadBar != null) {
        uploadBar.hidden = currentPath.length === 0;
    }
}

async function loadPath(path) {
    await ensureDrivesMounted();
    currentPath = path || "";
    renderPathBar(currentPath);
    updateUploadBar();
    const fileList = document.getElementById("fileList");
    fileList.textContent = "Loading...";

    try {
        const apiPath = currentPath
            ? "files.json?" + encodeURIComponent(currentPath)
            : "files.json";
        const response = await Promethe.api(apiPath);
        if (!response.ok) {
            throw new Error("request failed");
        }

        const data = await response.json();
        if (!data.entries || data.entries.length === 0) {
            fileList.innerHTML = "<p>This folder is empty.</p>";
            return;
        }

        let html = "<table><thead><tr><th>Name</th><th>Size</th><th class=\"prom-file-actions\">Actions</th></tr></thead><tbody>";
        for (let i = 0; i < data.entries.length; i++) {
            const entry = data.entries[i];
            const pathAttr = htmlAttr(entry.path);
            const nameAttr = htmlAttr(entry.name);
            const nameText = htmlText(entry.name);
            html += "<tr>";
            if (isNavigableEntry(entry)) {
                const linkClass = isDriveRoot(entry.path) ? "prom-file-dir prom-file-drive" : "prom-file-dir";
                html += "<td class=\"prom-file-name\"><a class=\"" + linkClass + "\" href=\"javascript:void(0)\" data-action=\"open\" data-path=\""
                    + pathAttr + "\">" + entryIcon(entry) + nameText + "</a></td>";
            } else {
                html += "<td class=\"prom-file-name\">" + entryIcon(entry) + nameText + "</td>";
            }
            html += "<td>" + (isNavigableEntry(entry) ? "-" : Promethe.formatBytes(entry.size)) + "</td>";
            html += "<td class=\"prom-file-actions\">";
            if (!isNavigableEntry(entry)) {
                html += "<a role=\"button\" class=\"secondary\" href=\"/api/downloadfile?"
                    + encodeURIComponent(entry.path) + "\" download=\"" + nameText + "\">Download</a>";
            }
            if (isDriveRoot(entry.path) === false) {
                if (!isNavigableEntry(entry)) {
                    html += " ";
                }
                html += "<a role=\"button\" class=\"outline\" href=\"javascript:void(0)\" data-action=\"rename\" data-path=\""
                    + pathAttr + "\" data-name=\"" + nameAttr + "\">Rename</a>";
                html += " <a role=\"button\" class=\"outline\" href=\"javascript:void(0)\" data-action=\"delete\" data-path=\""
                    + pathAttr + "\">Delete</a>";
            }
            html += "</td></tr>";
        }
        html += "</tbody></table>";
        fileList.innerHTML = html;
    } catch (error) {
        fileList.textContent = "Failed to connect.";
    }
}

async function deleteEntry(path) {
    if (!window.confirm("Delete " + path + "?")) {
        return;
    }

    try {
        const response = await Promethe.api("deletefile?" + encodeURIComponent(path));
        if (!response.ok) {
            throw new Error("delete failed");
        }
        Promethe.toast("Deleted.", "success");
        loadPath(currentPath);
    } catch (error) {
        Promethe.toast("Delete failed.", "error");
    }
}

async function createFolder() {
    if (!currentPath) {
        Promethe.toast("Open a drive first.", "error");
        return;
    }

    const name = await openNameDialog("New folder name", "");
    if (!isValidName(name)) {
        return;
    }

    const destination = currentPath.endsWith("\\") ? currentPath + name : currentPath + "\\" + name;

    try {
        const response = await Promethe.api("createdir?" + encodeURIComponent(destination));
        if (!response.ok) {
            throw new Error("create failed");
        }
        Promethe.toast("Folder created.", "success");
        loadPath(currentPath);
    } catch (error) {
        Promethe.toast("Could not create folder.", "error");
    }
}

async function renameEntry(path, currentName) {
    const name = await openNameDialog("Rename", currentName);
    if (!isValidName(name) || name === currentName) {
        return;
    }

    // Rename in place: same parent directory, new leaf name.
    const parent = path.substring(0, path.lastIndexOf("\\"));
    const destination = parent + "\\" + name;

    try {
        const data = new FormData();
        data.append('body', JSON.stringify({ source: path, dest: destination }));
        const response = await fetch("/api/rename", { method: 'POST', body: data });
        if (!response.ok) {
            throw new Error("rename failed");
        }
        Promethe.toast("Renamed.", "success");
        loadPath(currentPath);
    } catch (error) {
        Promethe.toast("Rename failed.", "error");
    }
}

async function uploadSelectedFile() {
    const input = document.getElementById("uploadFile");
    if (!input.files || input.files.length === 0) {
        Promethe.toast("Choose a file first.", "error");
        return;
    }

    if (!currentPath) {
        Promethe.toast("Open a folder before uploading.", "error");
        return;
    }

    const file = input.files[0];
    const destination = currentPath.endsWith("\\") ? currentPath + file.name : currentPath + "\\" + file.name;
    const formData = new FormData();
    formData.append("file", file);

    try {
        const response = await fetch("/api/uploadfile?" + encodeURIComponent(destination), {
            method: "POST",
            body: formData
        });
        if (!response.ok) {
            throw new Error("upload failed");
        }
        Promethe.toast("Upload complete.", "success");
        input.value = "";
        loadPath(currentPath);
    } catch (error) {
        Promethe.toast("Upload failed.", "error");
    }
}
