<div align="center">

# Team Resurgent Presents, PrometheOS
**A Modern Modchip OS for the Original Xbox**

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://github.com/Team-Resurgent/Repackinator/blob/main/LICENSE.md)
[![Discord](https://img.shields.io/badge/chat-on%20discord-7289da.svg?logo=discord)](https://discord.gg/VcdSfajQGK)

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/J3J7L5UMN)
[![Patreon](https://img.shields.io/badge/Patreon-F96854?style=for-the-badge&logo=patreon&logoColor=white)](https://www.patreon.com/teamresurgent)

</div>

<div>

Please see below for instructions regarding PrometheOS and how to use.

BEWARE: It is recommended you have a either another modchip (to hotswap) or a way to program the modchip incase you're no longer able to boot into PrometheOS

# Working Modchips

* Aladdin 1mb
* Aladdin 2mb
* Xchanger V2.5
* Xecuter 3 (Genuine *Purple* + Christmas Edition *Red & White*)
* Open Xenium
* Legacy Xenium
* Modxo (Works with Offical, YD-RP2040 & RP2040 Zero pico's)

## System Requirements
### Minimum
* Visual Studio 2003 With XDK (5933 Recommended)
* Visual Studio 2022

## How To Contribute

Create a PR and we will be more than happy to review and update appropriately

## Instructions

### Building PromethOS Web

* Open PrometheOSUtility\PrometheOSUtility.sln in VS2022 select PrometheOSWeb as startup project
* Build and run

Notes:

1) If changing any web files and want to test with the XBE you can then update the XBE using instructions described 'Packaging PromethOS firmware'
2)  In each of the js files there is a http://{ipaddress} change ipaddress to that of your xbox running PrometheOS to fully test.

### Building XBE / Testing

BEWARE: Certain actions will write to Xenium flash unless you disable Xenium flash by disabling the define ENABLE_XENIUM in xenium.cpp to simulate the flash instead.
  
* Open PrometheOSXbe\PrometheOSXbe.sln in VS2003
* Compile as Debug to test or Release for Packaging described in 'Packaging PromethOS firmware'

### Packaging PromethOS firmware

* Open PrometheOSUtility\PrometheOSUtility.sln in VS2022 select to build PrometheOSPacker as startup project
* Build and run
* Follow on screen prompts
* Flash your xenium to test (Beware it is recommended you have a xenium)

Notes:

1) Modify variable prometheosWebTestIp with IP used in web .js files (this is essential so that when ran on Xbox it uses local xbox's IP)
2) If you want to embed a installer logo modify the installerName variable appropriately
3) If you would like the packaged result to be uploaded to Xbox / Xenium Programmer uncomment the FTP section of code entering relevant FTP details 

Notes: 

1) You will need to provide mcpx_1.0.bin in the roms folder in order to run
2) The XEMU is a special build of XEMU which fully simulates a Xenium Modchip and is based upon intial code by Ryzee119 and extended upon to create a full implementation of the flash. (https://github.com/Team-Resurgent/xemu/tree/xenium)

Attribution:

This project include a small portion of software developed by MakeMHz<br/>
Original project: https://github.com/MakeMHz/xbox-hd-plus-app<br/>
Licensed under the GNU General Public License version 2. See LICENSE-GPLv2.md.

</div>
