async function screenshot(){await fetch("http://192.168.1.66/api/screenshot").catch(()=>{content.innerHTML="Failed to connect."}).then(n=>n.blob()).then(n=>{var r=window.URL.createObjectURL(n),t=document.createElement("img");t.src=r;t.alt="Screenshot";t.style="max-width: 100%; height: auto";const i=document.getElementById("content");i.innerHTML="";i.appendChild(t)})}window.onload=function(){screenshot()};