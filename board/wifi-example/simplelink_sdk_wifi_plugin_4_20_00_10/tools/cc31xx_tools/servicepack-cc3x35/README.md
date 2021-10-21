## Introduction
The SimpleLink™ Wi-Fi® CC3235 device is a single-chip micro-controller (MCU) with built-in Wi-Fi connectivity, created for the Internet of Things (IoT).
The CC3235 device is a wireless MCU that integrates a high-performance ARM® Cortex®-M4 MCU, allowing customers to develop an entire application with a single IC.

## Content and Documentation
This release include the ServicePack binary image to be programmed into CC3135/CC3235 devices.
**ServicePack must be applied both in production and development stage of the CC3135/CC3235 devices**

| File |  Notes |
| --- | --- |
| sp_4.7.0.3_3.1.0.5_3.1.0.26.bin | ServicePack binary for UniFlash |
| sp_4.7.0.3_3.1.0.5_3.1.0.26.ucf | ServicePack UCF for host driver API |
| sp_4.7.0.3_3.1.0.5_3.1.0.26.ucf.signed.bin | ServicePack UCF signature |


**Version information**

| Component |  Version |
| --- | --- |
| NWP | 4.7.0.3 |
| MAC | 3.1.0.5 |
| PHY | 3.1.0.26 |

**Note:**
This is a beta version, AP Transition (Agile) cannot be tested yet since the needed APs are missing and some more development is needed.

## What's New

* Beta version for Network Assisted Roaming:
	* Triggered roaming
		* Triggering mode can be enabled
		* Trigger threshold can be set
		* Roaming will occur when trigger threshold is reached
	* AP Transition (Agile)
		* WNM notification request will be sent when changing supported scan channels
		* RM capabilities, EXT capabilities and MBO IE are added to association request and probe request



## Upgrade and Compatibility Information

The ServicePack can be programmed using UniFlash utility.
Latest UniFlash utility can be downloaded from <http://www.ti.com/tool/UniFlash>. 

The ServicePack can also be flushed using host driver API's or OTA application 
(please refer for the SimpleLink CC32xx SDK for more information)

## Dependencies

This release requires the following software components and tools:

* UniFlash latest version - [Download page](http://www.ti.com/tool/UniFlash).
* The ServicePack is bounded to host driver 3.0.1.63

## Device Support
* CC3135
* CC3235S – Dual-Band 2.4 and 5-GHz Wi-Fi, MCU with 256KB of RAM, IoT networking security, device identity/keys as well as MCU level security such as file system encryption, user IP (MCU image) encryption, secure boot and debug security  
* CC3235SF - Dual-Band 2.4 and 5-GHz Wi-Fi, MCU with 1MB user-dedicated flash and 256KB of RAM, IoT networking security, device identity/keys as well as MCU level security such as file system encryption, user IP (MCU image) encryption, secure boot and debug  

**Evaluation Boards**
* CC3135\_BOOSTXL
* CC3235S\_LAUNCHXL
* CC3235SF\_LAUNCHXL


## Fixed Issues

${GEN3_FIXED_ISSUES}

## Known Issues

${GEN3_OPEN_ISSUES}

## Versioning

This product's version follows a version format, **M.mm.pp.bb**, where **M** is a single digit Major number, **mm** is 2 digit minor number, **pp** should be zero indicating official version and **b** is an unrestricted set of digits used as an incremented build counter.

## Technical Support and Product Updates

* Visit the [E2E Forum](https://e2e.ti.com/support/wireless_connectivity/simplelink_wifi_cc31xx_cc32xx/f/)
