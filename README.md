<div align="center">

# Team Resurgent Presents, PrometheOS
**A Modern Xenium OS for the Original Xbox**

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://github.com/Team-Resurgent/Repackinator/blob/main/LICENSE.md)
[![Discord](https://img.shields.io/badge/chat-on%20discord-7289da.svg?logo=discord)](https://discord.gg/VcdSfajQGK)

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/J3J7L5UMN)
[![Patreon](https://img.shields.io/badge/Patreon-F96854?style=for-the-badge&logo=patreon&logoColor=white)](https://www.patreon.com/teamresurgent)

</div>

<div>

Please see below for instructions regarding PrometheOS and how to use.

BEWARE: It is recommended you have a external xenium programmer incase your no longer able to boot into PrometheOS

## System Requirements
### Minimum
* Visual Studio 2003 With XDK (5933 Recommended)
* Visual Studio 2022

## How To Contribute

Create a PR and we will be more than happy to review and update appropriately

## Instructions

### Building PromethOS Web

* Open PrometheOSTools\PrometheOSTools.sln in VS2022 select PrometheOSWeb as startup project
* Build and run

Notes:

1) If changing any web files and want to test with the XBE you can then update the XBE using instructions described 'Packaging PromethOS firmware'
2)  In each of the js files there is a http://{ipaddress} change ipaddress to that of your xbox running PrometheOS to fully test.

### Building XBE / Testing

BEWARE: Certain actions will write to Xenium flash unless you disable Xenium flash by disabling the define ENABLE_XENIUM in xenium.cpp to simulate the flash instead.
  
* Open PackageXbe\PackageXbe.sln in VS2022
* Compile as Debug to test or Release for Packaging described in 'Packaging PromethOS firmware'

### Packaging PromethOS firmware

* Open PrometheOSTools\PrometheOSTools.sln in VS2022 select to build PrometheOSPacker as startup project
* Build and run
* Follow on screen prompts
* Flash your xenium to test (Beware it is recommended you have a xenium)

Notes:

1) Modify variable prometheosWebTestIp with IP used in web .js files (this is essential so that when ran on Xbox it uses local xbox's IP)
2) If you want to embed a installer logo modify the installerName variable appropriately
3) If you would like the packaged result to be uploaded to Xbox / Xenium Programmer uncomment the FTP section of code entering relevant FTP details 

</div>

