/*
 * OtaDropbox.h - Header file for OtaDropbox.c
 *
 *
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __OTA_DROPBOX_V2_H__
#define __OTA_DROPBOX_V2_H__

#ifdef    __cplusplus
extern "C" {
#endif

#include <ti/net/ota/source/CdnClient.h>

/* dropbox specific functions */
int16_t CdnDropboxV2_SendReqDir(int16_t SockId,
                                uint8_t *pSendBuf,
                                uint8_t *pServerName,
                                uint8_t *pVendorDir,
                                uint8_t *pVendorToken);
int16_t CdnDropboxV2_ParseRespDir(int16_t SockId,
                                  uint8_t *pRespBuf,
                                  OtaDirData_t *pOtaDirData);
int16_t CdnDropboxV2_SendReqFileUrl(int16_t SockId,
                                    uint8_t *pSendBuf,
                                    uint8_t *pServerName,
                                    uint8_t *pFileName,
                                    uint8_t *pVendorToken);
int16_t CdnDropboxV2_ParseRespFileUrl(uint16_t SockId,
                                      uint8_t *pRespBuf,
                                      uint8_t *pFileUrl,
                                      uint32_t FileUrlBufSize);
int16_t CdnDropboxV2_SendReqFileContent(int16_t SockId,
                                        uint8_t *pSendBuf,
                                        uint8_t *pFileServerName,
                                        uint8_t *pFileName);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __OTA_DROPBOX_V2_H__ */
